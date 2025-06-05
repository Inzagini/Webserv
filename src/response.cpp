#include "response.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server){
	int			statusCode = -1;
	std::string	bodyStr;
	std::string	statusText;
	std::string	filePath = "." + server.root + req.requestPath;

	if (req.requestPath == "/")
		filePath += server.index;
	std::cout << "[GET File path]: " << filePath <<std::endl;
	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent)
		return makeResponse(server, 404, bodyStr);
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	return makeResponse(server, 200, bodyStr);
}

/*
	parse the filename, file type, save the file to asigned location.
	If not define save to the root
*/
std::string	handlePost(const HttpRequest &req, const ServerConfig &server){
	std::string	filename;
	std::string	contentType;

	int	contentTypePos = req.body.find("Content-Type");
	if (contentTypePos != std::string::npos){
		int	contentTypePosEnd = req.body.substr(contentTypePos + 14).find("\r\n");
		contentType = req.body.substr(contentTypePos + 14, contentTypePosEnd);
	}

	if (contentType != "image/png" && contentType != "image/jpeg" && req.requestPath != "/upload")
		return makeResponse(server, 403, "Forbiden");

	int filenamePos = req.body.find("filename=\"");
	if (filenamePos != std::string::npos){
		int filenamePosEnd = req.body.substr(filenamePos + 10).find("\"");
		filename = req.body.substr(filenamePos + 10, filenamePosEnd);
	}

	if (filename.empty())
		filename = "default";

	std::string	filePath;
	std::string	savePath;
	if (req.location.uploadStore.empty())
		savePath = server.root;
	else
		savePath = req.location.uploadStore;

	filePath = "." + savePath + "/" + req.file;
	struct stat st;
	if (stat(("." + savePath).c_str(), &st) != 0){
		if (mkdir(("." + savePath).c_str(), 0755) != 0) {
			std::cerr << "Failed to create directory: " << "." + savePath << std::endl;
			return makeResponse(server, 500, "Failed to create upload directory");
		}
	}
	filePath = "." + savePath + "/" + filename;

	if (filePath.empty())
		return makeResponse(server, 404, "Failed to save file");

	if (writeToFile(req, server, filePath) == true)
		return makeResponse(server, 200, "File uploaded successfully");
	else
		return makeResponse(server, 500,  "Failed to save file");
}

/*
	try to find the file and delete
*/
std::string	handleDelete(const HttpRequest &req, const ServerConfig &server){
	std::string	filePath;
	std::string	savePath;
	if (req.location.uploadStore.empty())
		savePath = server.root;
	else
		savePath = req.location.uploadStore;

	filePath = "." + savePath + "/" + req.file;
	struct stat st;
	if (stat(filePath.c_str(), &st) != 0){
		std::cerr << "File not found\n";
		return makeResponse(server, 404, "File doesn't exist");
	}

	if (remove(filePath.c_str()) != 0) {
		std::cerr << "Fail to delete file: " << filePath << std::endl;
		return makeResponse(server, 500, "Failed to delete the file");
	}
	std::cout << "File Deleted: " + filePath << std::endl;
	return makeResponse(server, 200, "OK");
}

//will and a static page later
std::string methodNotAllowedResponse(const ServerConfig &server) {
	return makeResponse(server, 405, " Method Not Allowed");
}

std::string	makeResponse(const ServerConfig &server, int statusCode, std::string bodyStr)
{
	std::ostringstream	response;

	bodyStr = "<html><body><h1>" + bodyStr + "</h1></body></html>";
	response << "HTTP/1.1 " << statusCode << " ";
	response << server.httpStatusMsg.find(statusCode)->second <<"\r\n";
	if (statusCode >= 400){
		std::string body = ErrorContent(server, statusCode, bodyStr);
		if (!body.empty())
			bodyStr = body;
	}
	else if (statusCode >= 300 && statusCode < 400){
		response << "Location: " << "https://google.com" << "\r\n"
				<< "Content-Length: 0\r\n"
				<< "Connection: close\r\n"
				<< "\r\n";
		return response.str();
	}

	response << "Content-Length: " << bodyStr.size() << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "\r\n"
		<< bodyStr;

	return response.str();
}

bool	writeToFile(const HttpRequest &req, const ServerConfig &server, std::string filePath)
{
	int contentStart = req.body.find("\r\n\r\n");
	int contentEnd = req.body.substr(contentStart + 4).find("----");
	std::string	content = req.body.substr(contentStart + 4, contentEnd);

	std::ofstream	outFile(filePath.c_str(), std::ios::binary);
	if (outFile) {
		outFile.write(content.c_str(), content.size());
		outFile.close();
		std::cout << "File saved to: " << filePath << std::endl;
		return true;
	}
	else
		std::cerr << "Failed to save file: " << filePath << std::endl;
		return false;
}
