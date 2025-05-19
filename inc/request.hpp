#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

#include "server.hpp"

struct HttpRequest {
	std::string	method;
	std::string	path;
	std::string	version;
	std::map<std::string, std::string>	headers;
	std::string	body;
};

void printHttpRequest(const HttpRequest& req);
HttpRequest parseHttpRequest(char *rawInput);
std::string	handleRequest(const HttpRequest &req, const ServerConfig &server);
std::string	ErrorContent(ServerConfig server, int errorCode);

#endif
