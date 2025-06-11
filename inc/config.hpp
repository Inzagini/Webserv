#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
#include <iostream>

#define SIZE_MAX ((size_t) - 1)
#define DEFAULT_SIZE 2ULL * 1024 * 1024 * 1024

struct LocationConfig
{
	std::string					path;
	std::string					cgiPath;
	std::string					uploadStore;
	bool						redirect;
	int							redirectCode;
	std::string					redirectAddress;
	size_t						client_max_body_size;
	std::vector<std::string>	allowMethod;
};

struct ServerConfig
{
	int							listenPort;
	std::string					listenIP;
	std::string					serverName;
	std::string					root;
	std::string					index;
	std::string					redirectAddress;
	bool						redirect;
	int							redirectCode;
	size_t						client_max_body_size;
	std::map<int, std::string>	errorPages;
	std::map<int, std::string>	httpStatusMsg;
	std::vector<LocationConfig>	locations;

	typedef	std::vector<LocationConfig>::const_iterator const_iterator;
	const_iterator				locBegin() const {return locations.begin();}
	const_iterator				locEnd() const {return locations.end();}
};

class Config
{
	private:
		std::vector<ServerConfig>	servers;

	public:
		int							load(std::string filename);
		std::vector<ServerConfig>	&getServer();

	private:
		std::vector<std::string>	tokenize(const std::string& line, char delim);
		void						initServerBlock(ServerConfig &server);
		int							parseServerBlock(std::istream& file, ServerConfig &server);
		void						initLocationBlock(LocationConfig &server);
		int							parseLocationBlock(std::istream &file, LocationConfig &loc);
		void						setHttpStatusMsg(ServerConfig &server);
		size_t						parseBodySize(std::string &sizeStr);

};

std::string	trim(const std::string& s);
void		printTokens(const std::vector<std::string>& tokens); //to delete
#endif
