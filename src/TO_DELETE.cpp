
#include "config.hpp"
#include "request.hpp"

void printTokens(const std::vector<std::string>& tokens) {
	std::cout << "[Tokens]" << std::endl;

    for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        std::cout << *it << std::endl;
    }
	std::cout << std::endl;
}

void printHttpRequest(const HttpRequest& req) {
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.requestPath << std::endl;
    std::cout << "Version: " << req.version << std::endl;
    std::cout << "Headers:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }

    std::cout << "Body:" << std::endl;
    std::cout << req.body << std::endl;
}

void printQueryParams(const std::map<std::string, std::string>& queryParams) {
    for (std::map<std::string, std::string>::const_iterator it = queryParams.begin();
         it != queryParams.end(); ++it) {
        std::cout << it->first << " = " << it->second << std::endl;
    }
}


