#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "config.hpp"
#include <algorithm>

class Server
{
	private:
		std::vector<pollfd>	fds;
		std::vector<int>	serverFds;

	public:
		void	setServerFd(Config conf);
		int		connectionHandle(ServerConfig &server);

	private:
		void	clientHandle(int fd);
};

int	setSocket(ServerConfig &server);

#endif
