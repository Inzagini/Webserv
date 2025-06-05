#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <sys/stat.h>
#include "request.hpp"

struct HttpRequest;

std::string	handleGet(const HttpRequest &req, const ServerConfig &server);
std::string	methodNotAllowedResponse(const ServerConfig &server);
bool		writeToFile(const HttpRequest &req, const ServerConfig &server, std::string filePath);
std::string	makeResponse(const ServerConfig &server, int statusCode, std::string bodyStr);
#endif
