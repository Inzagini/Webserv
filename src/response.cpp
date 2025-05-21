#include "response.hpp"

std::string	handleGet(const HttpRequest &req, const ServerConfig &server){
	std::string	bodyStr;
	int			statusCode = -1;
	std::string statusText;
	std::string	filePath = "." + server.root + req.path;

	if (req.path == "/")
		filePath += "index.html";

	std::ifstream	fileContent(filePath.c_str());
	if (!fileContent){
		bodyStr = ErrorContent(server, 404, "Not Found");
		statusText = " Not Found";
		statusCode = 404;
	}
	else{
		std::ostringstream	body;
		body << fileContent.rdbuf();
		bodyStr = body.str();
		fileContent.close();
	}
	if (statusCode == -1){
		statusCode = 200;
		statusText = "OK";
	}

	return makeResponse(statusCode, statusText, bodyStr);
}

/*
	parse the filename, file type, save the file
*/
std::string	handlePost(const HttpRequest &req, const ServerConfig &server){
	std::string	filename;
	std::string	contentType;
	int filenamePos = req.body.find("filename=\"");
	if (filenamePos != std::string::npos){
		int filenamePosEnd = req.body.substr(filenamePos + 10).find("\"");
		filename = req.body.substr(filenamePos + 10, filenamePosEnd);
	}

	int	contentTypePos = req.body.find("Content-Type");
	if (contentTypePos != std::string::npos){
		int	contentTypePosEnd = req.body.substr(contentTypePos + 14).find("\r\n");
		std::string contentType = req.body.substr(contentTypePos + 14, contentTypePosEnd);
	}

	if (filename.empty())
		filename = "default";
	std::string filePath = "./app/uploads/" + filename;

	int contentStart = req.body.find("\r\n\r\n");
	int contentEnd = req.body.substr(contentStart + 4).find("----");
	std::string content = req.body.substr(contentStart + 4, contentEnd);

	if (content != "image/png" || content != "iamge/jpg")
		return makeResponse(403, "Forbiden", "<html><body><h1>Forbiden</h1></body></html>");
	std::ofstream outFile(filePath.c_str(), std::ios::binary);
	if (outFile) {
		outFile.write(content.c_str(), content.size());
		outFile.close();
		std::cout << "File saved to: " << filePath << std::endl;
		return makeResponse(200, "OK", "<html><body><h1>File uploaded successfully</h1></body></html>");
	}
	else {
		std::cerr << "Failed to save file: " << filePath << std::endl;
		return makeResponse(500, "Internal Server Error", "<html><body><h1>Failed to save file</h1></body></html>");
	}
}

//will and a static page later
std::string methodNotAllowedResponse(const ServerConfig &server) {
	std::string body = ErrorContent(server, 405, "Method not allowed");
	return makeResponse(405, "Method Not Allowed", body);
}

std::string	makeResponse(int statusCode, std::string statusText, std::string bodyStr)
{
	std::ostringstream	response;
	response << "HTTP/1.1 " << statusCode << statusText <<"\r\n"
		<< "Content-Length: " << bodyStr.size() << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "\r\n"
		<< bodyStr;

	return response.str();
}
