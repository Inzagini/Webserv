#include "utils.hpp"

void	logPrint(std::string type, std::string msg){
	std::time_t now = std::time(NULL);
	std::tm* localTime = std::localtime(&now);
	std::cout << CGREY << "["
			<< (1900 + localTime->tm_year) << "-"
			<< std::setw(2) << std::setfill('0') << (1 + localTime->tm_mon) << "-"
			<< std::setw(2) << std::setfill('0') << localTime->tm_mday
			<< " "
			<< std::setw(2) << std::setfill('0') << localTime->tm_hour << "-"
			<< std::setw(2) << std::setfill('0') << localTime->tm_min << "-"
			<< std::setw(2) << std::setfill('0') << localTime->tm_sec
			<< "] " << CRESET;

	std::string color;
	if (type == "INFO") color = CCYAN;
	else if (type == "WARN") color = CYELLOW;
	else if (type == "ERRO") color = CRED;
	else color = CRESET;
	std::cout << color << "[" << type << "] " << CRESET << "> ";

	std::cout << msg;
}

std::string generateUploadsListPage(const ServerConfig &server, const LocationConfig &location) {
	std::ostringstream html;
	std::string uploadsDir;

	if (!location.uploadStore.empty())
		uploadsDir = "." + location.uploadStore;
	else
		uploadsDir = "." + server.root;

	html << "<!DOCTYPE html><html lang=\"en\"><head>";
	html << "<meta charset=\"UTF-8\" />";
	html << "<title>File Upload</title>";
	html << "<style>"
			"body {"
				"font-family: Arial, sans-serif;"
				"text-align: center;"
				"padding-top: 50px;"
				"background-color: #f9f9f9;"
				"color: #333;"
			"}"
			"h1 { color: #2c3e50; }"
			"p { font-size: 1.2rem; }"
			"a { text-decoration: none; color: #3498db; font-weight: bold; }"
			"a:hover { text-decoration: underline; }"
			"button {"
			"padding: 10px 15px;"
			"font-size: 1rem;"
			"background-color: #3498db;"
			"color: white;"
			"border: none;"
			"cursor: pointer;"
			"}"
			"button:hover {"
			"background-color: #2980b9;"
			"}"
		  "</style>";
	html << "</head><body>";
	html << "<h1>Uploaded Files</h1>";

	html << "<table border='1' style='margin:auto;'><tr><th>Filename</th><th>Action</th></tr>";

	DIR *dir = opendir(uploadsDir.c_str());
	if (dir) {
		struct dirent *entry;
		bool hasFile = false;
		while ((entry = readdir(dir)) != NULL) {
			std::string fname = entry->d_name;
			if (fname == "." || fname == "..") continue;
			html << "<tr><td>" << fname << "</td>"
				 << "<td>"
				 << "<button onclick=\"deleteFile('" << fname << "')\">Delete</button>"
				 << "</td></tr>";
			hasFile = true;
		}
		if (!hasFile)
			html << "<tr><td colspan='2'>No files uploaded.</td></tr>";
		closedir(dir);
	} else {
		html << "<tr><td colspan='2'>Could not open upload directory.</td></tr>";
	}
	html << "</table>";
	html << "<br><a href='/'>Return to Homepage</a>";

	// JavaScript for DELETE request
	html << "<script type='text/javascript'>\n"
		 << "function deleteFile(fname) {"
		 << "  if (!confirm('Delete ' + fname + '?')) return;"
		 << "  var xhr = new XMLHttpRequest();"
		 << "  xhr.open('DELETE', '/upload/' + encodeURIComponent(fname), true);"
		 << "  xhr.onreadystatechange = function() {"
		 << "	if (xhr.readyState == 4) location.reload();"
		 << "  };"
		 << "  xhr.send();"
		 << "}"
		 << "</script>\n";

	html << "</body></html>";
	return html.str();
}

std::string	genereateDirectoryListing(const ServerConfig &server, const HttpRequest &req){
	std::ostringstream html;

	html << "<!DOCTYPE html><html><head><title>Index of " << req.requestPath << "</title></head>";
	html << "<body><h1>Index of " << req.requestPath << "</h1><hr>";
	html << "<table><tr><th>Name</th><th>Size</th></tr>";

	std::string dirPath = "." + req.location.uploadStore;

	DIR *dir = opendir(dirPath.c_str());
	if (dir){
		struct dirent *entry;
		bool hasFile = false;
		while ((entry = readdir(dir)) != NULL) {
			std::string fname = entry->d_name;
			if (fname == ".") continue;
			std::string fullPath = dirPath + "/" + fname;
			struct stat fileStat;

			html << "<tr><td>";
			if (fname == "..") {
				html << "<a href=\"../\">" << fname << "/</a>";
			} else if (stat(fullPath.c_str(), &fileStat) == 0) {
				if (S_ISDIR(fileStat.st_mode)) {
					html << "<a href=\"" << fname << "/\">" << fname << "/</a>";
				} else {
					html << "<a href=\"" << fname << "\">" << fname << "</a>";
				}
				html << "</td><td>" << fileStat.st_size << "</td>";
			} else {
				html << fname << "</td><td>-</td>";
			}
			html << "</tr>";
			hasFile = true;

		}
		if (!hasFile)
			html << "<tr><td colspan='2'>No files uploaded.</td></tr>";
		closedir(dir);
	}
	else
		return makeResponse(req, server, 404, "Folder not found", "");
	html << "</table><hr></body></html>";
	return makeResponse(req, server, 200, html.str(), "text/html");
}

bool	isDirectory(const std::string &path){
	struct stat pathStat;
	if (stat(path.c_str(), &pathStat) == 0){
		return S_ISDIR(pathStat.st_mode);
	}
	return false;
}

bool hasDirectoryReadPermission(const std::string& path) {
	return access(path.c_str(), R_OK) == 0;
}
