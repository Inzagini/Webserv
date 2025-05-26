#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <sys/stat.h>
#include <iostream>
#include <vector>

#include "request.hpp"
#include "server.hpp"
#include "response.hpp"

struct HttpRequest;

class cgi{

	private:
		std::vector<std::string>	env;

	public:
		std::string	handleCGI(const HttpRequest &req, const ServerConfig &server);
		bool	isCgiPath(std::string path);
};

#endif
