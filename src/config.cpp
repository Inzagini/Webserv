#include "config.hpp"

/*
 trim with spaces in the beginning and the end of server block
*/
std::string trim(const std::string& s)
{
	size_t	start = s.find_first_not_of(" \t\r\n");
	size_t	end = s.find_last_not_of(" \t\r\n");

	if (start == std::string::npos)
		return "";
	else
		return s.substr(start, end - start + 1);
}

std::vector<std::string> Config::tokenize(const std::string &line, char delim) {
	std::istringstream iss(line);
	std::string token;
	std::vector<std::string> tokens;

	while (iss >> token) {
		if (!token.empty() && token[token.size() - 1] == delim)
			token.erase(token.size() - 1);
		tokens.push_back(token);
	}
	return tokens;
}

/*
	load the entire file to the string.
	find server block and parse then save to the array of servers
*/
int	Config::load(std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
		throw std::runtime_error("Can't open the file: " + filename);

	std::string line;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		if (line == "server {"){
			ServerConfig server;
			this->initServerBlock(server);
			this->parseServerBlock(file, server);
			this->setHttpStatusMsg(server);
			servers.push_back(server);
		}
	}
	return EXIT_SUCCESS;
}

/*
	parser server block current follow rigid structure
*/
int	Config::parseServerBlock(std::istream &file, ServerConfig &server){
	std::string line;

	while (std::getline(file, line)) {
		line = trim(line);
		if (line == "}") break;

		std::vector<std::string> tokens = tokenize(line, ';');
		if (tokens.empty()) continue;

		if (tokens[0] == "location" && tokens.size() > 1){
			LocationConfig loc;
			loc.path = tokens[1];
			this->initLocationBlock(loc);
			this->parseLocationBlock(file, loc);
			server.locations.push_back(loc);
		}
		else if (tokens[0] == "listen" && tokens.size() > 1){
			size_t colon = tokens[1].find(':');
			if (colon == std::string::npos)
				throw std::runtime_error("Missing ':' in " + tokens[1]);
			server.listenIP = tokens[1].substr(0, colon);
			server.listenPort = std::atoi(tokens[1].substr(colon + 1).c_str());
		}
		else if (tokens[0] == "server_name" && tokens.size() == 2)
			server.serverName = tokens[1];
		else if (tokens[0] == "root" && tokens.size() == 2)
			server.root = tokens[1];
		else if (tokens[0] == "index" && tokens.size() == 2)
			server.index = tokens[1];
		else if (tokens[0] == "error_page" && tokens.size() == 3){
			int code = std::atoi(tokens[1].c_str());
			server.errorPages[code] = tokens[2];
		}
		else if (tokens[0] == "return" && tokens.size() == 3){
			server.redirect = true;
			server.redirectCode = std::atoi(tokens[1].c_str());
			server.redirectAddress = tokens[2];
		}
		else if (tokens[0] == "client_max_body_size" && tokens.size() == 2){
			server.client_max_body_size = parseBodySize(tokens[1]);
		}
		else
			throw std::runtime_error("Error in key: " + tokens[0]);
	}
	return EXIT_SUCCESS;
}

/*
	parse location blocks need checks for tokens
*/
int	Config::parseLocationBlock(std::istream &file, LocationConfig &loc){
	std::string line;

	while (std::getline(file, line)) {
		line = trim(line);
		if (line == "}") break;

		std::vector<std::string> tokens = tokenize(line, ';');
		if (tokens.empty()) continue;

		if (tokens[0] == "allow_method") {
			for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end(); ++it)
				loc.allowMethod.push_back(*it);
		}
		else if(tokens[0] == "index" && tokens.size() == 2){
			loc.index = tokens[1];
		}
		else if (tokens[0] == "upload_store" && tokens.size() > 1)
			loc.uploadStore = tokens[1];
		else if (tokens[0] == "cgi_pass" && tokens.size() > 1)
			loc.cgiPath = tokens[1];
		else if (tokens[0] == "return" && tokens.size() == 3){
			loc.redirect = true;
			loc.redirectCode = std::atoi(tokens[1].c_str());
			loc.redirectAddress = tokens[2];
		}
		else if (tokens[0] == "client_max_body_size" && tokens.size() == 2){
			loc.client_max_body_size = parseBodySize(tokens[1]);
		}
		else
			throw std::runtime_error("Error in key: " + tokens[0]);
	}
	return EXIT_SUCCESS;
}

std::vector<ServerConfig> &Config::getServer(){
	return servers;
}

void	Config::setHttpStatusMsg(ServerConfig &server){
	server.httpStatusMsg[200] = "OK";
	server.httpStatusMsg[201] = "Created";
	server.httpStatusMsg[204] = "No Content";
	server.httpStatusMsg[301] = "Moved Permanently";
	server.httpStatusMsg[302] = "Found";
	server.httpStatusMsg[400] = "Bad Request";
	server.httpStatusMsg[401] = "Unauthorized";
	server.httpStatusMsg[403] = "Forbidden";
	server.httpStatusMsg[404] = "Not Found";
	server.httpStatusMsg[405] = "Method Not Allowed";
	server.httpStatusMsg[500] = "Internal Server Error";
	server.httpStatusMsg[501] = "Not Implemented";
	server.httpStatusMsg[502] = "Bad Gateway";
	server.httpStatusMsg[503] = "Service Unavailable";
}

size_t	Config::parseBodySize(std::string &sizeStr){
	size_t	size = 0;
	size_t i = -1;

	while (++i < sizeStr.size() && isdigit(sizeStr[i])){
		int digit = sizeStr[i] - '0';
		if (((SIZE_MAX - digit) / 10) < size) {
			throw std::runtime_error("Error: client max body size to large");
		}
		size = size * 10 + digit;
	}

	if (sizeStr.size() - i > 1)
		throw std::runtime_error("Error: client max body size incorrect format");
	else if (sizeStr.size() - i == 0){
		if (size > DEFAULT_SIZE)
			throw std::runtime_error("Error: client max body size is bigge than max allowed (2GB)");
	}
	else{
		if (sizeStr[i] == 'K')
			size *= 1028;
		else if (sizeStr[i] == 'M')
			size *= 1028 * 1028;
		else if (sizeStr[i] == 'G')
			size *= 1028 * 1028 * 1028;
		else
			throw std::runtime_error("Error: Unknow size suffix");
	}

	return size;
}

void	Config::initServerBlock(ServerConfig &server){
	server.listenPort = -1;
	server.listenIP = "";
	server.serverName = "";
	server.root = "";
	server.index = "";
	server.redirectAddress = "";
	server.redirect = false;
	server.redirectCode = -1;
	server.client_max_body_size = -1;
	server.errorPages = std::map<int, std::string> ();
	server.httpStatusMsg = std::map<int, std::string> ();
	server.locations = std::vector<LocationConfig> ();
}

void	Config::initLocationBlock(LocationConfig &loc){
	loc.index = "";
	loc.cgiPath = "";
	loc.uploadStore = "";
	loc.redirect = false;
	loc.redirectCode = -1;
	loc.redirectAddress = "";
	loc.client_max_body_size = -1;
	loc.allowMethod = std::vector<std::string> ();
}
