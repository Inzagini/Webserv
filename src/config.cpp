#include "config.hpp"

/*
	load the entire file to the string.
	find server block
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

int	serverConfig::load(std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
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
			std::cout << "found server block" << std::endl;
			serverConfig server;
			// parseServerBlock(file, server);
			// servers.push_back(server);
		}
	}


	return 0;
}
