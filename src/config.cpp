#include "config.hpp"

/*
	load the entire file to the string.
	find server block and parse
*/

/*
 trim with spaces in the beginning and the end of server block
*/
std::string serverConfig::trim(const std::string& s)
{
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	else
		return s.substr(start, end - start + 1);
}

std::vector<std::string> serverConfig::tokenize(const std::string &line) {
	std::istringstream iss(line);
	std::string token;
	std::vector<std::string> tokens;
	while (iss >> token) {
		if (!token.empty() && token[token.size() - 1] == ';')
			token.erase(token.size() - 1);
		tokens.push_back(token);
	}

	// printTokens(tokens);
	return tokens;
}

int	serverConfig::load(std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file) {
		std::cerr << "Can't open the file: " << filename << std::endl;
		return (EXIT_FAILURE);
	}
	std::string line;

	while (std::getline(file, line))
	{
		line = trim(line);

		if (line.empty() || line[0] == '#') continue;

		if (line == "server {")
		{
			serverConfig server;
			parseServerBlock(file, server);
		}
	}
	return EXIT_SUCCESS;
}

/*
	parser server block current follow rigid structure
*/
int	serverConfig::parseServerBlock(std::istream &file, serverConfig &server)
{
	std::string line;
	while (std::getline(file, line)) {
		line = trim(line);
		if (line == "}") break;

		std::vector<std::string> tokens = tokenize(line);
		if (tokens.empty()) continue;

		if (tokens[0] == "location" && tokens.size() > 1)
		{
			LocationConfig loc;
			loc.path = tokens[1];
			parseLocationBlock(file, loc);
			server.locations.push_back(loc);
		}
		else if (tokens[0] == "listen" && tokens.size() > 1)
		{
			size_t colon = tokens[1].find(':');
			if (colon == std::string::npos) {
				std::cerr << "Missing ':' in " << tokens[1] << std::endl;
				return EXIT_FAILURE;
			}
			server.ip = tokens[1].substr(0, colon);
			server.port = std::atoi(tokens[1].substr(colon + 1).c_str());
		}
		else if (tokens[0] == "server_name" && tokens.size() > 1){
			server.server_name = tokens[1];
		}
		else if (tokens[0] == "root" && tokens.size() > 1){
			server.root = tokens[1];
		}
		else if (tokens[0] == "index" && tokens.size() > 1){
			server.index = tokens[1];
		}
		else if (tokens[0] == "error_page" && tokens.size() > 1){
			int code = std::atoi(tokens[1].c_str());
			server.errorPages[code] = tokens[2];
		}
		else {
			std::cerr << "Unknow key: " << tokens[0] << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

/*
	parse location blocks need checks for tokens
*/
int	serverConfig::parseLocationBlock(std::istream &file, LocationConfig &loc)
{
	std::string line;
	while (std::getline(file, line)){
		line = trim(line);
		if (line == "}") break;

		std::vector<std::string> tokens = tokenize(line);
		if (tokens.empty()) continue;

		if (tokens[0] == "allow_method"){
			for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end(); ++it)
				loc.allow_method.push_back(*it);
		}
		else if (tokens[0] == "upload_store" && tokens.size() > 1){
			loc.upload_store = tokens[1];
		}
		else if (tokens[0] == "cgi_pass" && tokens.size() > 1){
			loc.cgiPath == tokens[1];
		}
		else{
			std::cerr << "Unknow key: " << tokens[0] << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
