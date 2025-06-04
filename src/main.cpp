#include "config.hpp"
#include "server.hpp"
#include "request.hpp"

int main ()
{
	Config conf;
	std::string filename = std::string("./conf.d/default.conf");
	if (conf.load(filename) == EXIT_FAILURE)
		return EXIT_FAILURE;
	Server server;
	server.setServerFd(conf);
	server.connectionHandle(conf.getServer()[0]);

	return EXIT_SUCCESS;
}
