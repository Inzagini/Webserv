#include "cgi.hpp"

/*
	check for .py
*/
bool	cgi::isCgiPath(const HttpRequest &req, const ServerConfig &server){
	std::string::size_type	cgiPos = req.file.find(".py");
	if (cgiPos == std::string::npos)
		return false;

	std::string	savePath;
	if (req.location.cgiPath.empty())
		savePath = server.root;
	else
		savePath = req.location.cgiPath;
	this->fullPath = "." + savePath + "/" +req.file;
	if (req.file[cgiPos + 3] == '\0'){
		std::string	param;
		for (std::map<std::string, std::string>::const_iterator it = req.queryParams.begin();
			it != req.queryParams.end(); it++){
				param = it->first + "=" + it->second;
				env.push_back(strdup(param.c_str()));
		}
		std::ostringstream cl;
		cl << "CONTENT_LENGTH=" << req.body.size();
		env.push_back(strdup(cl.str().c_str()));
		env.push_back(NULL);
		return true;
	}
	return false;
}

std::string	cgi::handleCGI(const HttpRequest &req, const ServerConfig &server){
	std::string	response = "";
	struct stat	st;

	if (stat(this->fullPath.c_str(), &st) != 0)
		return makeResponse(req, server, 404, "Not found", "");

	if (pipe(this->inFd) < 0)
		return makeResponse(req, server, 500, "Failed to create Pipe", "");
	if (pipe(this->outFd) < 0){
		close(this->inFd[1]); close(this->inFd[0]);
		return makeResponse(req, server, 500, "Failed to create Pipe", "");
	}

	if (executor(req, response) == -1)
		return makeResponse(req, server, 500, "CGI executor fail", "");
	else
		return this->createCGIResponse(req, server, response);
}

int	cgi::executor(const HttpRequest &req, std::string &response){
	pid_t pid = fork();
	if (pid < 0){
		close(this->inFd[1]); close(this->inFd[0]);
		close(this->outFd[1]); close(this->outFd[0]);
		return -1;
	}
	if (pid == 0){
		if (req.method == "POST" && req.body.size() > 0)
			dup2(this->inFd[0], STDIN_FILENO);
		dup2(this->outFd[1], STDOUT_FILENO);
		close(this->inFd[0]); close(this->inFd[1]);
		close(this->outFd[1]); close(this->outFd[0]);

		char *args[] = { (char*)"/usr/bin/python3", const_cast<char *>(this->fullPath.c_str()), NULL};
		execve(args[0], args, const_cast<char* const*>(&env[0]));
		exit(1);
	}
	close(this->inFd[0]); close(this->outFd[1]);
	if (req.method == "POST" && req.body.size() > 0){
		if (write(inFd[1], req.body.c_str(), req.body.size()) != (ssize_t)req.body.size())
			return -1;
	}
	close(this->inFd[1]);

	time_t		startTime = time(NULL);

	if (this->readFromPipe(pid, startTime, response) == -1)
		return (close(this->outFd[0]), -1);

	close(this->outFd[0]);

	pid_t	result;
	int		status;
	while (time(NULL) - startTime < CGI_TIMEOUT) {
		result = waitpid(pid, &status, WNOHANG);
		if (result > 0)
			break;
		else if (result == -1)
			return -1;
		usleep(100000); // 100ms
	}

	// If process still hasn't exited, kill it
	if (result == 0) {
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0)
			return -1;
	}
	else if (WIFSIGNALED(status)){
		return -1;
	}

	return 1;
}

int	cgi::readFromPipe(pid_t pid, time_t startTime, std::string &response){
	char		buffer[4096];
	ssize_t		len;
	fd_set		readfds;
	struct		timeval timeout;

	while (true){
		if (time(NULL) - startTime >= CGI_TIMEOUT) {
			close(this->outFd[0]);
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			return -1;
		}
		FD_ZERO(&readfds);
		FD_SET(outFd[0], &readfds);
		timeout.tv_sec = 1;  // 1 second timeout for each select call
		timeout.tv_usec = 0;

		int select_result = select(outFd[0] + 1, &readfds, NULL, NULL, &timeout);
		if (select_result == -1) {
			close(this->outFd[0]);
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			return -1;
		}
		else if (select_result == 0) // timeout occurred, continue loop to check total timeout
			continue;
		else {
			len = read(outFd[0], buffer, sizeof(buffer)); // Data is available to read
			if (len > 0)
				response.append(buffer, len);
			else if (len == 0)
				break; // EOF reached
			else {
				close(this->outFd[0]); // read error
				kill(pid, SIGKILL);
				waitpid(pid, NULL, 0);
				return -1;
			}
		}
	}
	return 1;
}

std::string	cgi::createCGIResponse(const HttpRequest &req, const ServerConfig &server, std::string &response){
	std::ostringstream oss;
	size_t	headerEnd = response.find("\n\n");
	size_t	seperatorLen = 2;
	if (headerEnd == std::string::npos){
		headerEnd = response.find("\r\n\r\n");
		seperatorLen = 4;
		if (headerEnd == std::string::npos){
			logPrint("ERRO", "CGI script did not provide proper headers\n");
			return makeResponse(req, server, 500, "CGI script did not provide proper headers", "");
		}
	}

	std::string headers = response.substr(0, headerEnd);
	if (headers.find("Content-Type:") == std::string::npos){
		logPrint("ERRO", "CGI script missing required headers\n");
		return makeResponse(req, server, 500, "CGI script missing required headers", "");
	}

	std::string statusLine = "200 OK";
	std::string body = response.substr(headerEnd + seperatorLen);
	size_t	statusPos = response.find("Status:");
	if (statusPos != std::string::npos){
		size_t statusStart = statusPos + 7;
		size_t statusEnd = response.find("\r\n", statusStart);
		if (statusEnd == std::string::npos)
			statusEnd = response.find("\n", statusStart);
		statusLine = trim(response.substr(statusStart, statusEnd - statusStart));

		if (headers[statusEnd] == '\n')
			headers.erase(statusPos, statusEnd + 1 - statusPos);
		else if (headers.substr(statusEnd, 2) == "\r\n")
			headers.erase(statusPos, statusEnd + 2 - statusPos);
	}
	oss << "HTTP/1.1 " + statusLine + "\n";
	oss << "Connection: close\r\n";
	oss << "Content-Length: " << body.size() << "\r\n";

	std::istringstream headerStream(headers);
	std::string line;
	while (std::getline(headerStream, line)) {
		if (!line.empty() && line[line.length()-1] == '\r')
			line.erase(line.length()-1);
		if (!line.empty())
			oss << line << "\r\n";
	}
	oss << "\r\n" << body;

	std::ostringstream msg;
	msg << "Server responded " << statusLine << std::endl;
	logPrint("INFO", msg.str());
	return oss.str();
}

cgi::~cgi(){
	for (std::vector<const char *>::iterator it = this->env.begin(); it != this->env.end(); it++){
		free ((void *)*it);
	}
	env.clear();
}
