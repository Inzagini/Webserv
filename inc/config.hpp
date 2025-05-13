#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>

class config
{
	private:
		std::string	listen;
		int			port;
		std::string	cgiPath;
		std::string	htmlPath;
		std::string	errPpath;


};

#endif
