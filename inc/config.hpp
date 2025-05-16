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
	std::string	try_file;
	std::string	path;
	std::string	cgiTrue;
	std::string	cgiPath;
	std::string	upload_store;
};

class serverConfig
{
	private:
		std::string					listen;
		std::string					ip;
		int							port;
		std::string					server_name;
		std::string					root;
		std::string					index;
		std::map<int, std::string>	errorPages;
		std::vector<LocationConfig>	locations;

	public:
		int		load(std::string filename);
		void	configParser();

		std::string				 		trim(const std::string& s);
		std::vector<std::string> 		tokenize(const std::string& line);

		int	parseServerBlock(std::istream& file, serverConfig &server);
		int	parseLocationBlock(std::istream &file, serverConfig &server);

};

#endif
