#include "response.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server){
	std::string	bodyStr;
	std::string	filePath = "." + server.root;

	if (req.requestPath == "/")
		filePath = filePath + req.requestPath + server.index;
	else if (req.requestPath == req.path && isDirectory("." + req.location.uploadStore)){

		if (hasDirectoryReadPermission("." + req.location.uploadStore) == false)
			return makeResponse(req, server, 403, "Folder - Permision denied", "text/html");

		if (req.location.index != "")
			filePath = filePath + "/" + req.location.index;
		else
			return genereateDirectoryListing(server, req);
	}
	else
		filePath += req.requestPath;

	if (req.requestPath.size() > 12 &&
		req.requestPath.substr(req.requestPath.size() - 12) == "/delete.html")
		return makeResponse(req, server, 200, generateUploadsListPage(server, req.location), "text/html");

	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent)
		return makeResponse(req, server, 404, bodyStr, "");
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	return makeResponse(req, server, 200, bodyStr, "text/html");
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
		return makeResponse(req, server, 413, "Size of content is bigger than allowed", "text/html");
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
			logPrint("ERRO", msg.str());
			return makeResponse(req, server, 500, "Failed to create upload directory", "text/html");
		}
	}
	std::string	filePath = "." + savePath + "/" + filename;

	if (filePath.empty()){
		msg << "Failed to save file\n";
		logPrint("WARN", msg.str());
		return makeResponse(req, server, 404, "Failed to save file", "");
	}

	if (writeToFile(req, filePath) == true)
		return makeResponse(req, server, 200, "File uploaded successfully", "text/html");
	else
		return makeResponse(req, server, 500,  "Failed to save file", "text/html");

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

	filePath = "." + savePath + "/" + urlDecode(req.file);
	struct stat st;
	if (stat(filePath.c_str(), &st) != 0){
		msg << "File not found\n";
		logPrint("WARN", msg.str());
		return makeResponse(req, server, 404, "File doesn't exist", "");
	}

	if (remove(filePath.c_str()) != 0) {
		msg << "Fail to delete file: " << filePath << std::endl;
		logPrint("ERRO", msg.str());
		return makeResponse(req, server, 500, "Failed to delete the file", "");
	}
	return makeResponse(req, server, 200, "OK", "");
}

//will and a static page later
std::string methodNotAllowedResponse(const HttpRequest &req, const ServerConfig &server) {
	return makeResponse(req, server, 405, "Client requested with not allowed method", "");
}

std::string makeResponse(const HttpRequest &req, const ServerConfig &server, int statusCode, std::string bodyStr, std::string contentTypeOrRedirect) {
 	std::ostringstream response;
	std::ostringstream msg;

 	msg << "Server responded " << statusCode << " ";
	if (statusCode < 300){
		msg << std::endl;
		logPrint("INFO", msg.str());
	}
	else if (statusCode >= 300 && statusCode < 400){
		msg << "client redirected to " << contentTypeOrRedirect << std::endl;
 	 	logPrint("INFO", msg.str());
	}
	else if (statusCode >= 400 && statusCode < 500){
		msg << bodyStr << std::endl;
 	 	logPrint("WARN", msg.str());
	}
	else {
		msg << bodyStr << std::endl;
		logPrint("ERRO", msg.str());
	}

	response << "HTTP/1.1 " << statusCode << " ";
 	response << server.httpStatusMsg.find(statusCode)->second << "\r\n";

 	if (statusCode >= 300 && statusCode < 400) {
 	 	response << "Location: " << contentTypeOrRedirect << "\r\n"
 	 	 	 	 << "Content-Length: 0\r\n"
 	 	 	 	 << "Connection: close\r\n"
 	 	 	 	 << "\r\n";
 	 	return response.str();
 	}

 	if (statusCode >= 400) {
 	 	std::string errorBody = ErrorContent(server, statusCode, bodyStr);
 	 	if (!errorBody.empty())
 	 	 	bodyStr = errorBody;
 	 	contentTypeOrRedirect = "text/html"; // now allowed to assign
 	}

 	response << "Content-Length: " << bodyStr.size() << "\r\n";
 	response << "Content-Type: " << contentTypeOrRedirect << "\r\n";

	std::string connectionType = "close";
	std::map<std::string, std::string>::const_iterator it = req.headers.find("Connection");
	if (it != req.headers.end()){
		connectionType = trim(it->second);
	}
 	response << "Connection: " << connectionType << "\r\n";
 	response << "\r\n";

 	response << bodyStr;
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

std::string	urlDecode(std::string fileName){
	std::ostringstream out;
	for (size_t i = 0; i < fileName.length(); i++){
		if (fileName[i] == '%' && i + 2 < fileName.length()){
			std::istringstream iss (fileName.substr(i + 1, 2));
			int hex = 0;
			if (iss >> std::hex >> hex)
				out << static_cast<char>(hex);
			i += 2;
		}
		else if (fileName[i] == '+')
			out << ' ';
		else
			out << fileName[i];
	}
	return out.str();
}
