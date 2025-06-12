#include "response.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server){
	std::string	bodyStr;
	std::string	statusText;
	std::string	filePath = "." + server.root + req.requestPath;

	if (req.requestPath == "/")
		filePath += server.index;
	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent)
		return makeResponse(server, 404, bodyStr, "");
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	return makeResponse(server, 200, bodyStr, "");
}

/*
	parse the filename, file type, save the file to asigned location.
	If not define save to the root
*/
std::string	handlePost(const HttpRequest &req, const ServerConfig &server){
	std::string	filename;
	std::string	savePath;
	std::ostringstream	msg;

	if (checkContentSize(req, server)){
		msg << "Size of content is bigger than allowed" << std::endl;
		logPrint("WARN", msg.str());
		return makeResponse(server, 403, "Size of content is bigger than allowed", "");
	}

	std::string::size_type filenamePos = req.body.find("filename=\"");
	if (filenamePos != std::string::npos){
		int filenamePosEnd = req.body.substr(filenamePos + 10).find("\"");
		filename = req.body.substr(filenamePos + 10, filenamePosEnd);
	}

	if (filename.empty())
		filename = "default";

	if (req.location.uploadStore.empty())
		savePath = server.root;
	else
		savePath = req.location.uploadStore;

	struct stat st;
	if (stat(("." + savePath).c_str(), &st) != 0){
		if (mkdir(("." + savePath).c_str(), 0755) != 0) {
			msg << "Failed to create directory: " << "." + savePath << std::endl;
			logPrint("ERROR", msg.str());
			return makeResponse(server, 500, "Failed to create upload directory", "");
		}
	}
	std::string	filePath = "." + savePath + "/" + filename;

	if (filePath.empty()){
		msg << "Failed to save file\n";
		logPrint("WARN", msg.str());
		return makeResponse(server, 404, "Failed to save file", "");
	}

	if (writeToFile(req, filePath) == true)
		return makeResponse(server, 200, "File uploaded successfully", "");
	else
		return makeResponse(server, 500,  "Failed to save file", "");

}

/*
	try to find the file and delete
*/
std::string	handleDelete(const HttpRequest &req, const ServerConfig &server){
	std::string			filePath;
	std::string			savePath;
	std::ostringstream	msg;

	if (req.location.uploadStore.empty())
		savePath = server.root;
	else
		savePath = req.location.uploadStore;

	filePath = "." + savePath + "/" + req.file;
	struct stat st;
	if (stat(filePath.c_str(), &st) != 0){
		msg << "File not found\n";
		logPrint("WARN", msg.str());
		return makeResponse(server, 404, "File doesn't exist", "");
	}

	if (remove(filePath.c_str()) != 0) {
		msg << "Fail to delete file: " << filePath << std::endl;
		logPrint("ERROR", msg.str());
		return makeResponse(server, 500, "Failed to delete the file", "");
	}
	return makeResponse(server, 200, "OK", "");
}

//will and a static page later
std::string methodNotAllowedResponse(const ServerConfig &server) {
	std::ostringstream	msg;
	msg << "Client requested with not allowed method\n";
	logPrint("INFO", msg.str());
	return makeResponse(server, 405, " Method Not Allowed", "");
}

std::string	makeResponse(const ServerConfig &server, int statusCode, std::string bodyStr, std::string redir)
{
	std::ostringstream	response;
	std::ostringstream	msg;
	msg << "Server responded " << statusCode;

	bodyStr = "<html><body><h1>" + bodyStr + "</h1></body></html>";
	response << "HTTP/1.1 " << statusCode << " ";
	response << server.httpStatusMsg.find(statusCode)->second <<"\r\n";
	if (statusCode >= 400){
		std::string body = ErrorContent(server, statusCode, bodyStr);
		if (!body.empty())
			bodyStr = body;
	}
	else if (statusCode >= 300 && statusCode < 400){
		response << "Location: " << redir << "\r\n"
				<< "Content-Length: 0\r\n"
				<< "Connection: close\r\n"
				<< "\r\n";
		msg << " client redirected to " << redir <<std::endl;
		logPrint("INFO", msg.str());
		return response.str();
	}

	response << "Content-Length: " << bodyStr.size() << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "\r\n"
		<< bodyStr;
	msg << std::endl;
	logPrint("INFO", msg.str());
	return response.str();
}

bool	writeToFile(const HttpRequest &req, std::string filePath)
{
	int			contentStart = req.body.find("\r\n\r\n");
	int			contentEnd = req.body.substr(contentStart + 4).find("----");
	std::string	content = req.body.substr(contentStart + 4, contentEnd);

	std::ofstream		outFile(filePath.c_str(), std::ios::binary);
	std::ostringstream	msg;
	if (outFile) {
		outFile << content;
		outFile.close();
		msg << "File saved to: " << filePath << std::endl;
		logPrint("INFO", msg.str());
		return true;
	}
	else{
		msg << "Failed to save file: " << filePath << std::endl;
		logPrint("WARN", msg.str());
		return false;
	}
}

int	checkContentSize(const HttpRequest &req, const ServerConfig &server){
	if (req.location.client_max_body_size != (size_t)-1){
		if (req.location.client_max_body_size < req.contentLength)
			return 1;
	}
	else if (server.client_max_body_size != (size_t)-1){
		if (server.client_max_body_size < req.contentLength)
			return 1;
	}
	else{
		if (DEFAULT_SIZE < req.contentLength)
			return 1;
	}
	return 0;
}
