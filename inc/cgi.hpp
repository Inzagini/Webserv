#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <string.h>

#include "request.hpp"
#include "server.hpp"
#include "response.hpp"

struct	HttpRequest;

class	cgi{

	private:
		std::vector<const char *>	env;
		std::string					fullPath;

	public:
		~cgi();

	public:
		std::string	handleCGI(const HttpRequest &req, const ServerConfig &server);
		bool	isCgiPath(const HttpRequest &req, const ServerConfig &server);
		void	executor(const HttpRequest &req, int inFd[2], int outFd[2],pid_t pid, std::string &response);
};
#endif
