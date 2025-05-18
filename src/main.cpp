#include "config.hpp"
#include "server.hpp"
#include "request.hpp"

int main ()
{
	Config conf;
	std::string filename = std::string("./conf.d/default.conf");
	conf.load(filename);

	connectionHandle(conf.getServer()[0]);

	return EXIT_SUCCESS;
}
