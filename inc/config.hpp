#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
/*
	error to cover 400, 403, 404, 405, 413
					500, 502, 503
*/

struct LocationConfig
{
	std::string	path;
	std::vector<std::string>	allow_method;
	std::string	cgiPath;
	std::string	upload_store;
};

struct ServerConfig
{
	std::string					listen_ip;
	int							listen_port;
	std::string					server_name;
	std::string					root;
	std::string					index;
	std::map<int, std::string>	errorPages;
	std::vector<LocationConfig>	locations;
};

class Config
{
	private:
		std::vector<ServerConfig> servers;

	public:
		int				load(std::string filename);
		std::vector<ServerConfig> &getServer() {
			return servers;
		}

	private:
		std::vector<std::string> 		tokenize(const std::string& line, char delim);
		int	parseServerBlock(std::istream& file, ServerConfig &server);
		int	parseLocationBlock(std::istream &file, LocationConfig &loc);

};

std::string				 		trim(const std::string& s);
void printTokens(const std::vector<std::string>& tokens); //to delete
#endif
