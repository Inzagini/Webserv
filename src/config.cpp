#include "config.hpp"


int	config::load(std::string filename)
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
		std::cout << line << std::endl;
	}
	return 0;
}
