#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <dirent.h>
#include "config.hpp"

void	logPrint(std::string type, std::string msg);
std::string generateUploadsListPage(const ServerConfig &server, const LocationConfig &location);

#endif
