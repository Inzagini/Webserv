#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
/*
	error to cover 400, 403, 404, 405, 413
					500, 502, 503
*/

struct LocationConfig
{
	std::string	root;
	std::string	path;
	std::string	index;
	std::string	cgiPath;
	std::string	upload_store;
};

class config
{
	private:
		std::string					listen;
		int							port;
		std::map<int, std::string>	errorPages;
		std::vector<LocationConfig>	locations;

	public:
		int							load(std::string filename);
		void							configParser();
		std::vector<LocationConfig>&	getLocations() const;
};

#endif
