#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

struct HttpRequest {
	std::string	method;
	std::string	path;
	std::string	version;
	std::map<std::string, std::string>	headers;
	std::string	body;
};

HttpRequest parseHttpRequest(char *rawInput);
void printHttpRequest(const HttpRequest& req);
#endif
