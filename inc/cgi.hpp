#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include <sys/select.h>

#include "request.hpp"
#include "server.hpp"
#include "response.hpp"

# define CGI_TIMEOUT 5

struct	HttpRequest;

class	cgi{

	private:
		std::vector<const char *>	env;
		std::string					fullPath;
		int					inFd[2], outFd[2], errFd[2];

	private:
		int		readFromPipe(pid_t pid, time_t startTime, std::string &response);
		std::string	createCGIResponse(const HttpRequest &req, const ServerConfig &server, std::string &response);

	public:
		~cgi();
		std::string	handleCGI(const HttpRequest &req, const ServerConfig &server);
		bool	isCgiPath(const HttpRequest &req, const ServerConfig &server);
		int		executor(const HttpRequest &req, std::string &response);
};
#endif
