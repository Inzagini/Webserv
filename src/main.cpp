#include "config.hpp"
#include "server.hpp"


int main ()
{
	Config conf;
	std::string filename = std::string("./conf.d/default.conf");
	conf.load(filename);

	// int server_id = setSocket(conf);
	// connectionHandle(server_id);
	return EXIT_SUCCESS;
}
