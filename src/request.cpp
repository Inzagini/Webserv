#include "request.hpp"

std::string	handleRequest(HttpRequest &req, const ServerConfig &server){
	cgi			cgiO;
	bool		validPath = false;
	std::string	bodyStr;
	std::string	path = req.path;

	if (isValidHTTPMethod(req.method) == false)
		return makeResponse(req, server, 400, "Bad Request - Invalid HTTP Method", "text/html");

	if (path.size() > 1 && path[path.size() - 1] == '/')
		path = path.erase(path.size() - 1);

	for (ServerConfig::const_iterator it = server.locBegin();
		it != server.locEnd(); it++){
		if (it->path == path){
			validPath = true;
			req.location = static_cast<LocationConfig>(*it);
			break;
		}
	}

	if (server.redirect == true || req.location.redirect == true){
		if (server.redirect == true)
			return makeResponse(req, server, server.redirectCode, "Redirect", server.redirectAddress);
		else if (req.location.redirect == true){
			return makeResponse(req, server, req.location.redirectCode, "Redirect", req.location.redirectAddress);
		}
	}

	if (!validPath)
		return makeResponse(req, server, 404, "Not found", "");
	if (!isMethodAllowed(req.location.allowMethod, req.method))
		return methodNotAllowedResponse(req, server);

	if (cgiO.isCgiPath(req, server))
		return cgiO.handleCGI(req, server);

	if (req.method == "GET")
		return handleGet(req, server);
	else if (req.method == "POST")
		return handlePost(req, server);
	else if (req.method == "DELETE")
		return handleDelete(req, server);
	else
		return methodNotAllowedResponse(req, server);
}


bool	isMethodAllowed(const std::vector<std::string> &allowedMethod, std::string method){

	for (std::vector<std::string>::const_iterator it = allowedMethod.begin();
		it != allowedMethod.end(); it++){
			if (*it == method)
				return true;
		}
	return false;
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
	std::ostringstream	msg;

	if (path.empty()){
		msg << "No error file - Using default error" << std::endl;
		logPrint("WARN", msg.str());
		bodyStr = "<html><body><h1> Error: " + errMsg + "</h1></body></html>";
		return bodyStr;
	}
	path = "." + path;
	std::ifstream fileContent(path.c_str());
	if (!fileContent){
		msg << "File not found: " << path << " ";
		msg << "using default error" << std::endl;
		logPrint("WARN", msg.str());
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

bool	isValidHTTPMethod(const std::string &method){
	return (method == "GET" || method == "POST" || method == "DELETE" ||
			method == "PUT" || method == "HEAD" || method == "OPTION");
}


