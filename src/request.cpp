#include "request.hpp"
#include "config.hpp"
#include "response.hpp"
#include "cgi.hpp"

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

	int queryPos = req.requestPath.find('?');
	std::string pathOnly = req.requestPath;
	std::string query = "";
	if (queryPos != std::string::npos){
		query = req.requestPath.substr(queryPos + 1);
		pathOnly = req.requestPath.substr(0, queryPos);
	}

	int lastSlashPos = pathOnly.rfind('/');
	if (lastSlashPos != std::string::npos){
		req.path = pathOnly.substr(0, lastSlashPos + 1);
		req.file = pathOnly.substr(lastSlashPos + 1);
		req.queryParams = parseQuery(query);
	}
	else{
		req.path = "/";
		req.file = pathOnly;
	}

	std::cout << "[path]: " << req.path << " | " << req.file << std::endl;

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
		int		contentLenght = std::atoi(it->second.c_str());
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
	cgi			cgiO;

	if (req.method == "GET"){
		if (cgiO.isCgiPath(req.requestPath)){
			return cgiO.handleCGI(req, server);
		}
		return handleGet(req, server);
	}
	else if (req.method == "POST"){
		return handlePost(req, server);
	}
	else if (req.method == "DELETE"){
		return handleDelete(req, server);
	}
	else{
		return methodNotAllowedResponse(server);
	}
}


/*
	rootPath is the /app/html
	need Method check and if it is allowed
	maybe later split to different method then call dedicated function
*/
std::string	ErrorContent(ServerConfig server, int errorCode, std::string errMsg)
{
	std::string	body;
	std::string	bodyStr;
	std::string	path = server.errorPages[errorCode];

	if (path.empty()){
		std::cout << "Using default error" << std::endl;
		bodyStr = "<html><body><h1>"+ errMsg + "</h1></body></html>";
		return bodyStr;
	}
	path = "." + path;
	std::ifstream fileContent(path.c_str());
	if (!fileContent){
		std::cout << "File not found: " << path << std::endl;
		std::cout << "Using default error" << std::endl;
		bodyStr = "<html><body><h1> " + errMsg + "</h1></body></html>";
	}
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	return bodyStr;
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
