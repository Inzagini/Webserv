#include "request.hpp"

/*
	parse the request to 3 part each seperated by empty line
	requeset
	headers
	body
*/
HttpRequest parseHttpRequest(const char *rawInput)
{
	HttpRequest			req;
	std::istringstream	stream(rawInput);
	std::string			line;

	std::getline(stream, line);
	std::istringstream	requestLine(line);
	requestLine >> req.method >> req.requestPath >> req.version;

	parsePathQuery(req);

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

	std::map<std::string, std::string>::iterator it = req.headers.find("Content-Length");
	if (it != req.headers.end()){
		int		contentLenght = std::atoi(it->second.c_str());
		char	*buffer = new char[contentLenght + 1];
		stream.read(buffer, contentLenght);
		buffer[contentLenght] = '\0';
		req.body = buffer;
		delete[] buffer;
	}
	return req;
}

void	parsePathQuery(HttpRequest &req){
	int queryPos = req.requestPath.find('?');
	std::string pathOnly = req.requestPath;
	std::string query = "";
	if (queryPos != std::string::npos){
		query = req.requestPath.substr(queryPos + 1);
		pathOnly = req.requestPath.substr(0, queryPos);
	}

	int lastSlashPos = pathOnly.rfind('/');
	if (lastSlashPos != std::string::npos){
		if (pathOnly.find('.') != std::string::npos){
			req.path = pathOnly.substr(0, lastSlashPos + 1);
			req.file = pathOnly.substr(lastSlashPos + 1);
		}
		else
			req.path = pathOnly.substr(lastSlashPos);
		req.queryParams = parseQuery(query);
	}
	else{
		req.path = "/";
		req.file = pathOnly;
	}
}

std::map<std::string, std::string>	parseQuery(const std::string &s){
	std::map<std::string, std::string>	params;
	std::vector<std::string>			pairs = split(s, '&');

	for(size_t i = 0; i < pairs.size(); i++){
		std::string key, value;
		int	equalPos = pairs[i].find('=');
		if (equalPos != std::string::npos){
			key = pairs[i].substr(0, equalPos);
			value = pairs[i].substr(equalPos + 1);
		}
		else{
			key = pairs[i];
			value = "";
		}
		params[key] = value;
	}
	return params;
}

std::vector<std::string>	split(const std::string &s, char delim){
	std::string	word;
	std::stringstream ss(s);
	std::vector<std::string>	tokens;

	while (std::getline(ss, word, delim)){
		tokens.push_back(word);
	}
	return tokens;
}
