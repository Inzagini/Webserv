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
	std::string	cgiPath;
	std::string	uploadStore;
	bool		redirect;
	int			redirectCode;
	std::string	redirectAddress;
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
		int							parseServerBlock(std::istream& file, ServerConfig &server);
		int							parseLocationBlock(std::istream &file, LocationConfig &loc);
		void						setHttpStatusMsg(ServerConfig &server);


};

std::string	trim(const std::string& s);
void		printTokens(const std::vector<std::string>& tokens); //to delete
#endif
