#include "request.hpp"
#include "config.hpp"

HttpRequest parseHttpRequest(char *rawInput)
{
	HttpRequest req;
	std::istringstream stream(rawInput);
	std::string line;

	//request
	std::getline(stream, line);
	std::istringstream requestLine(line);
	requestLine >> req.method >> req.path >> req.version;

	//header
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

	//body
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

/*
	rootPath is the /app
	need Method check and if it is allowed
	maybe later split to different method then call dedicated function
*/
std::string	serveFileRequest(int client_fd, const HttpRequest &req, const std::string &rootPath){

	std::string	filePath = "." + rootPath + req.path;

	if (req.path == "/")
		filePath += "index.html";

	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent){
		//404 not found
	}

	std::ostringstream	body;
	body << fileContent.rdbuf();
	std::string bodyStr = body.str();
	fileContent.close();

	//need dedicated respond and header builder
	std::ostringstream	response;
	response << "HTTP/1.1 200 OK\r\n"
		<< "Content-Length: " << bodyStr.size() << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "\r\n"
		<< bodyStr;

	return response.str();
}
