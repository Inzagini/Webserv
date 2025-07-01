#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <sys/stat.h>
#include "request.hpp"
#include "utils.hpp"

struct HttpRequest;

std::string	handleGet(const HttpRequest &req, const ServerConfig &server);
std::string	methodNotAllowedResponse(const HttpRequest &req, const ServerConfig &server);
bool		writeToFile(const HttpRequest &req, std::string filePath);
std::string	makeResponse(const HttpRequest &req, const ServerConfig &server, int statusCode, std::string bodyStr, std::string redir);
int	checkContentSize(const HttpRequest &req, const ServerConfig &server);
std::string	urlDecode(std::string fileName);
#endif
