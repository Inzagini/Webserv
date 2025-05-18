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
