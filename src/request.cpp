#include "request.hpp"
#include "config.hpp"
#include "response.hpp"

/*
	parse the request to 3 part each seperated by empty line
	requeset
	headers
	body
*/
HttpRequest parseHttpRequest(char *rawInput)
{
	HttpRequest req;
	std::istringstream stream(rawInput);
	std::string line;

	std::getline(stream, line);
	std::istringstream requestLine(line);
	requestLine >> req.method >> req.path >> req.version;

	while (std::getline(stream, line)){
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		size_t pos = line.find(':');
		if (pos != std::string::npos){
			std::string	key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			req.headers[key] = value;
		}
	}

	std::map<std::string, std::string>::iterator it = req.headers.find("Content-Lenght");
	if (it != req.headers.end()){
		int	contentLenght = std::atoi(it->second.c_str());
		char	*buffer = new char[contentLenght + 1];
		stream.read(buffer, contentLenght);
		buffer[contentLenght] = '\0';
		req.body = buffer;
		delete[] buffer;
	}
	return req;
}

std::string	handleRequest(const HttpRequest &req, const ServerConfig &server)
{
	std::string	bodyStr;

	if (req.method == "GET"){
		return handleGet(req, server);
	}
	else
		return methodNotAllowedResponse(req, server);
}


/*
	rootPath is the /app/html
	need Method check and if it is allowed
	maybe later split to different method then call dedicated function
*/


std::string	ErrorContent(ServerConfig server, int errorCode)
{
	std::string	body;
	std::string	bodyStr;
	std::string	path = server.errorPages[errorCode];
	if (path.empty()){
		std::cout << "File not found: " << path << std::endl;
		std::ostringstream oss;
		oss << errorCode;
		bodyStr = "<html><body><h1> " + oss.str() + " Not Found</h1></body></html>";
		return bodyStr;
	}
	path = "." + path;
	std::ifstream fileContent(path.c_str());
	if (!fileContent){
		std::cout << "File not found: " << path << std::endl;
		std::ostringstream oss;
		oss << errorCode;
		bodyStr = "<html><body><h1> " + oss.str() + " Not Found</h1></body></html>";
	}
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	return bodyStr;
}
