#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

#include "config.hpp"
#include "server.hpp"
#include "cgi.hpp"
#include "response.hpp"

struct HttpRequest {
	std::string	method;
	std::string	requestPath;
	std::string	path;
	std::string	file;
	std::string	version;
	size_t		contentLength;
	LocationConfig	location;
	std::map<std::string, std::string>	headers;
	std::map<std::string, std::string> queryParams;
	std::string	body;
};

struct LocationCongig;

void		printHttpRequest(const HttpRequest& req);
void		printQueryParams(const std::map<std::string, std::string>& queryParams);

HttpRequest	parseHttpRequest(const char *rawInput);
void		parsePathQuery(HttpRequest &req);
bool		isMethodAllowed(const std::vector<std::string> &allowedMethod, std::string method);

std::string	handleRequest(HttpRequest &req, const ServerConfig &server);
std::string	handlePost(const HttpRequest &req, const ServerConfig &server);
std::string	handleDelete(const HttpRequest &req, const ServerConfig &server);
std::string	ErrorContent(ServerConfig server, int errorCode, std::string errMsg);


std::vector<std::string>	split(const std::string &s, char delim);
std::map<std::string, std::string>	parseQuery(const std::string &s);
#endif
