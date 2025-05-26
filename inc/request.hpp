#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

#include "server.hpp"
#include "cgi.hpp"

struct HttpRequest {
	std::string	method;
	std::string	path;
	std::string	version;
	std::map<std::string, std::string>	headers;
	std::string	body;
};

void printHttpRequest(const HttpRequest& req);
HttpRequest	parseHttpRequest(const char *rawInput);
std::string	handleRequest(const HttpRequest &req, const ServerConfig &server);
std::string	handlePost(const HttpRequest &req, const ServerConfig &server);
std::string	handleDelete(const HttpRequest &req, const ServerConfig &server);
std::string	ErrorContent(ServerConfig server, int errorCode, std::string errMsg);

#endif
