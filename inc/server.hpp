#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <algorithm>
#include "config.hpp"
#include "request.hpp"

struct HttpRequest;

class Server
{
	private:
		std::vector<pollfd>	fds;
		std::vector<int>	serverFds;
		std::map<int, std::string> buffers;
		std::map<int, bool> headerParsed;
		std::map<int, size_t> expectedBodyLen;
		std::map<int, HttpRequest> parsedRequest;

	public:
		void	setServerFd(Config conf);
		int		connectionHandle(ServerConfig &server);

	private:
		void	clientHandle(int fd);
		void	headerParser(int fd);
		void	ReqRespHandle(int fd, ServerConfig &server);
		void	clientDisconnect(int fd, int i);
};

int	setSocket(ServerConfig &server);

#endif
