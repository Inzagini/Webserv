#include "config.hpp"
#include "server.hpp"
#include "request.hpp"

int main (int argc, char **argv)
{
	try{
		if (argc == 2){
			std::string filename(argv[1]);
			Config conf;
			if (conf.load(filename) == EXIT_FAILURE)
				return EXIT_FAILURE;
			Server server;
			server.setServerFd(conf);
			server.connectionHandle();
		}
		else if (argc == 1)
			throw std::runtime_error("Error: missing config file");
		else
			throw std::runtime_error("Error: more than one argument");
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
	}
	return EXIT_SUCCESS;
}
