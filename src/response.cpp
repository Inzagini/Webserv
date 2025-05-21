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
		bodyStr = ErrorContent(server, 404);
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
	std::map<std::string, std::string>::const_iterator it = req.headers.find("Content-Type");
	// std::cout << "[Body]: "<< req.body.size() << "\n" <<req.body << std::endl;
	std::string filename;
	int filenamePos = req.body.find("filename=\"");
	if (filenamePos != std::string::npos){
		int filenamePosEnd = req.body.substr(filenamePos + 10).find("\"");
		filename = req.body.substr(filenamePos + 10, filenamePosEnd);
		std::cout << "Filename: " << filename << std::endl;
	}
	if (filename.empty())
		filename = "default";
	std::string filePath = "./app/uploads/" + filename;

	int contentStart = req.body.find("\r\n\r\n");
	int contentEnd = req.body.substr(contentStart + 4).find("----");
	std::string content = req.body.substr(contentStart + 4, contentEnd);

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
std::string methodNotAllowedResponse() {
	std::string body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
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
