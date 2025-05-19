#include "response.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server){
	std::string	bodyStr;
	int			statusCode = -1;
	std::string statusText;
	std::string	filePath = "." + server.root + req.path;

	if (req.path == "/")
		filePath += "index.html";

	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent){
		bodyStr = ErrorContent(server, 404);
		statusText = " 404 Not Found";
		statusCode = 404;
	}
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	if (statusCode == -1){
		statusCode = 200;
		statusText = "OK";
	}

	return makeResponse(statusCode, statusText, bodyStr);
}

//will and a static page later
std::string methodNotAllowedResponse(const HttpRequest &req, const ServerConfig &server) {

	std::string	bodyStr = ErrorContent(server, 405);
	if (bodyStr.empty())
		bodyStr = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
	return makeResponse(405, "Method Not Allowed", bodyStr);
}

std::string	makeResponse(int statusCode, std::string statusText, std::string bodyStr)
{
	std::ostringstream	response;
	response << "HTTP/1.1 " << statusCode << " " << statusText <<"\r\n"
		<< "Content-Length: " << bodyStr.size() << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "\r\n"
		<< bodyStr;

	return response.str();
}
