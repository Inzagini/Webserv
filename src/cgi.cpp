#include "cgi.hpp"

/*
	check for .py
*/
bool	cgi::isCgiPath(const HttpRequest &req, const ServerConfig &server){
	int	cgiPos = req.file.find(".py");
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
		env.push_back(NULL);
		return true;
	}
	return false;
}

std::string	cgi::handleCGI(const HttpRequest &req, const ServerConfig &server){
	std::string	response = "";
	int	fd[2];
	struct stat	st;

	if (stat(this->fullPath.c_str(), &st) != 0)
		return makeResponse(server, 404, "Not found", "");

	if (pipe(fd) < 0)
		return makeResponse(server, 500, "Failed to create Pipe", "");
	pid_t pid = fork();
	if (pid < 0){
		close(fd[1]);
		close(fd[0]);
		return makeResponse(server, 500, "Failed to fork", "");
	}
	executor(req, fd, pid, response);
	std::cout << "[Responded]: " << response << std::endl;
	return makeResponse(server, 200, response, "");
}

void	cgi::executor(const HttpRequest &req, int fd[2], pid_t pid,std::string &response){
	if (pid == 0){
		if (req.method == "POST" && req.body.size() > 0)
			dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		char *args[] = { (char*)"/usr/bin/python3", const_cast<char *>(this->fullPath.c_str()), NULL};
		execve(args[0], args, const_cast<char* const*>(&env[0]));
		exit(1);
	}

	if (req.method == "POST" && req.body.size() > 0)
		write(fd[1], req.body.c_str(), req.body.size());
	close(fd[1]);

	char		buffer[4096];
	ssize_t		len;
	while ((len = read(fd[0], buffer, sizeof(buffer))) > 0)
		response.append(buffer, len);
	close(fd[0]);
}

cgi::~cgi(){
	for (std::vector<const char *>::iterator it = this->env.begin(); it != this->env.end(); it++){
		free ((void *)*it);
	}
	env.clear();
}
