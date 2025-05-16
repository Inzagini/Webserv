#include "config.hpp"

int main ()
{
	serverConfig conf;
	std::string filename = std::string("./conf.d/default.conf");
	conf.load(filename);

}
