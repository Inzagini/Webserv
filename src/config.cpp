#include "config.hpp"


int	config::load(std::string filename)
{
	std::ifstream file(filename);
	if (!file)
		return (EXIT_FAILURE);
}
