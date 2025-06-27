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
	int			inFd[2];
	int			outFd[2];
	struct stat	st;

	if (stat(this->fullPath.c_str(), &st) != 0)
		return makeResponse(server, 404, "Not found", "");

	if (pipe(inFd) < 0)
		return makeResponse(server, 500, "Failed to create Pipe", "");
	if (pipe(outFd) < 0){
		close(inFd[1]); close(inFd[0]);
		return makeResponse(server, 500, "Failed to create Pipe", "");
	}

	if (executor(req, inFd, outFd, response) == -1)
		return makeResponse(server, 500, "CGI executor fail", "");
	else
		return makeResponse(server, 200, response, "");
}

int	cgi::executor(const HttpRequest &req, int inFd[2], int outFd[2], std::string &response){
	pid_t pid = fork();
	if (pid < 0){
		close(inFd[1]); close(inFd[0]);
		close(outFd[1]); close(outFd[0]);
		return -1;
	}
	if (pid == 0){
		if (req.method == "POST" && req.body.size() > 0)
			dup2(inFd[0], STDIN_FILENO);
		dup2(outFd[1], STDOUT_FILENO);
		close(inFd[0]); close(inFd[1]);
		close(outFd[1]); close(outFd[0]);

		char *args[] = { (char*)"/usr/bin/python3", const_cast<char *>(this->fullPath.c_str()), NULL};
		execve(args[0], args, const_cast<char* const*>(&env[0]));
		exit(1);
	}
	close(inFd[0]); close(outFd[1]);
	if (req.method == "POST" && req.body.size() > 0){
		if (write(inFd[1], req.body.c_str(), req.body.size()) != (ssize_t)req.body.size())
			return -1;
	}
	close(inFd[1]);

	time_t		startTime = time(NULL);

	if (this->readFromPipe(pid, startTime, outFd, response) == -1)
		return (close(outFd[0]), -1);

	close(outFd[0]);

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

	return 1;
}

int	cgi::readFromPipe(pid_t pid, time_t startTime, int outFd[2], std::string &response){
	char		buffer[4096];
	ssize_t		len;
	fd_set		readfds;
	struct		timeval timeout;

	while (true){
		if (time(NULL) - startTime >= CGI_TIMEOUT) {
			close(outFd[0]);
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
			close(outFd[0]);
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
				close(outFd[0]); // read error
				kill(pid, SIGKILL);
				waitpid(pid, NULL, 0);
				return -1;
			}
		}
	}
	return 1;
}

cgi::~cgi(){
	for (std::vector<const char *>::iterator it = this->env.begin(); it != this->env.end(); it++){
		free ((void *)*it);
	}
	env.clear();
}
