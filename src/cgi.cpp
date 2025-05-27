#include "cgi.hpp"

/*
	maybe change this to the
*/
bool	cgi::isCgiPath(std::string path){
	int	cgipos = path.find("/cgi/");
	if (cgipos == std::string::npos)
		return false;

	this->fullPath = "./app/cgi/" + path.substr(cgipos + 5);
	struct stat	st;
	if (stat(this->fullPath.c_str(), &st) == 0)
		return true;
	return false;
}

std::string	cgi::handleCGI(const HttpRequest &req, const ServerConfig &server){
	std::string	response = "";
	int	out[2];

	if (pipe(out) < 0)
		return ""; //need some kind of the error

	pid_t pid = fork();
	if (pid < 0)
		return ""; //need some kind of error

	if (pid == 0){
		dup2(out[1], STDOUT_FILENO);
		close(out[1]);
		close(out[0]);

		char *args[] = { (char*)"/usr/bin/python3", const_cast<char *>(this->fullPath.c_str()), NULL};
		execve(args[0], args, NULL);
		exit(1);
	}
	close(out[1]);

	char		buffer[4096];
	ssize_t		len;
	while ((len = read(out[0], buffer, sizeof(buffer))) > 0)
		response.append(buffer, len);
	close(out[0]);

	std::cout << "[Response]: " << response << std::endl;
	return makeResponse(server, 200, " OK ",response);
}
