#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include "request.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server);
std::string methodNotAllowedResponse();

std::string	makeResponse(int statusCode, std::string statusText, std::string bodyStr);
#endif
