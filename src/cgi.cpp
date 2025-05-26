#include "cgi.hpp"

bool	cgi::isCgiPath(std::string path){
	int	cgipos = path.find("/cgi/");
	if (cgipos == std::string::npos)
		return false;

	std::string fullPath = "./app/cgi/" + path.substr(cgipos + 5);
	struct stat	st;
	if (stat(fullPath.c_str(), &st) == 0)
		return true;
	return false;
}

std::string	cgi::handleCGI(const HttpRequest &req, const ServerConfig &server){
	std::string	response = "";
	std::cout << "Here\n";
	return makeResponse(server, 200, " OK ",response);
}
