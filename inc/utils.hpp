#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <dirent.h>
#include "config.hpp"
#include "request.hpp"

# define CRESET		"\033[0m"
# define CRED		"\033[1;31m"
# define CGREEN		"\033[1;32m"
# define CYELLOW	"\033[1;33m"
# define CBLUE		"\033[1;34m"
# define CMAGENTA	"\033[1;35m"
# define CCYAN		"\033[1;36m"
# define CGREY		"\033[90m"

struct HttpRequest;

void	logPrint(std::string type, std::string msg);
bool	isDirectory(const std::string &path);
bool	hasDirectoryReadPermission(const std::string& path);
std::string	genereateDirectoryListing(const ServerConfig &server, const HttpRequest &req);
std::string generateUploadsListPage(const ServerConfig &server, const LocationConfig &location);

#endif
