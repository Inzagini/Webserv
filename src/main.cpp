#include "config.hpp"

int main ()
{
	config conf;
	std::string filename = std::string("./conf.d/default.conf");
	conf.load(filename);

}
