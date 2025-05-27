#include "response.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server){
	int			statusCode = -1;
	std::string	bodyStr;
	std::string	statusText;
	std::string	filePath = "." + server.root + req.requestPath;

	if (req.requestPath == "/")
		filePath += server.index;

	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent){
		return makeResponse(server, 404, " Not Found", bodyStr);
	}
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	return makeResponse(server, 200, " OK", bodyStr);
}

/*
	parse the filename, file type, save the file to the /app/uploads
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
		return makeResponse(server, 403, "Forbiden", "Forbiden");

	int filenamePos = req.body.find("filename=\"");
	if (filenamePos != std::string::npos){
		int filenamePosEnd = req.body.substr(filenamePos + 10).find("\"");
		filename = req.body.substr(filenamePos + 10, filenamePosEnd);
	}

	if (filename.empty())
		filename = "default";

	std::string filePath;
	for (std::vector<LocationConfig>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++){
		if (it->path == "/upload"){
			struct stat st;
			if (stat(("." + it->upload_store).c_str(), &st) != 0){
				if (mkdir(("." + it->upload_store).c_str(), 0755) != 0) {
					std::cerr << "Failed to create directory: " << "." + it->upload_store<< std::endl;
					return makeResponse(server, 500, "Internal Server Error", "Failed to create upload directory");
				}
			}
			filePath = "." + it->upload_store + "/" + filename;
		}
	}

	if (filePath.empty())
		return makeResponse(server, 404, "Not found", "Failed to save file");

	std::string response = writeToFile(req, server, filePath);
	return response;
}

std::string	handleDelete(const HttpRequest &req, const ServerConfig &server){
	std::cout << req.requestPath << std::endl;
	std::string	filePath;
	struct stat st;
	for (std::vector<LocationConfig>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++){
		if (it->path == "/upload"){
			struct stat st;
			filePath = "." + it->upload_store + "/" + req.requestPath;
			if (stat(filePath.c_str(), &st) != 0){
				return makeResponse(server, 404, "Not found", "File doesn't exist");
			}
		}
	}

	if (remove(filePath.c_str()) != 0) {
		std::cerr << "Faild to delete file: " << filePath << std::endl;
		return makeResponse(server, 500, "Internal Server Error", "Failed to delete the file");
	}
	return makeResponse(server, 200, "OK", "OK");
}

//will and a static page later
std::string methodNotAllowedResponse(const ServerConfig &server) {
	return makeResponse(server, 405, " Method Not Allowed", " Method Not Allowed");
}

std::string	makeResponse(const ServerConfig &server, int statusCode, std::string statusText, std::string bodyStr)
{
	std::ostringstream	response;

	bodyStr = "<html><body><h1>" + bodyStr + "</h1></body></html>";
	if (statusCode >= 400){
		std::string body = ErrorContent(server, statusCode, bodyStr);
		if (!body.empty())
			bodyStr = body;
	}
	response << "HTTP/1.1 " << statusCode << statusText <<"\r\n"
		<< "Content-Length: " << bodyStr.size() << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "\r\n"
		<< bodyStr;

	return response.str();
}

std::string	writeToFile(const HttpRequest &req, const ServerConfig &server, std::string filePath)
{
	int contentStart = req.body.find("\r\n\r\n");
	int contentEnd = req.body.substr(contentStart + 4).find("----");
	std::string	content = req.body.substr(contentStart + 4, contentEnd);

	std::ofstream	outFile(filePath.c_str(), std::ios::binary);
	if (outFile) {
		outFile.write(content.c_str(), content.size());
		outFile.close();
		std::cout << "File saved to: " << filePath << std::endl;
		return makeResponse(server, 200, " OK", "File uploaded successfully");
	}
	else {
		std::cerr << "Failed to save file: " << filePath << std::endl;
		return makeResponse(server, 500, "Internal Server Error", "Failed to save file");
	}
}
