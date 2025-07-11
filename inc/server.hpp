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
#include "utils.hpp"

struct HttpRequest;

enum ClientState{
	READING,
	PREPARE_RESPONSE,
	SEND_RESPONSE
};

class Server
{
	private:
		std::vector<pollfd>			fds;
		std::vector<int>			serverFds;
		std::map<int, std::string>	buffers;
		std::map<int, bool>			headerParsed;
		std::map<int, size_t>		expectedBodyLen;
		std::map<int, HttpRequest>	parsedRequest;
		std::map<int, ClientState>	clientState;
		std::map<int, std::vector<ServerConfig> >	serverFdToConfig;
		std::map<int, std::vector<ServerConfig> >	clientFdToConfig;
		std::map<int, std::string> responseQueue;

	public:
		~Server();
		void	setServerFd(Config conf);
		int		connectionHandle();

	private:
		bool	isServerCheck(int fd);
		void	clientHandle(int serverFd);
		void	headerParser(int clientFd);
		void	clientDisconnect(int clientFD, int i);
		void	prepareResponse(int clientFD);
		void	cleanClientData(int clientFD);
};

int	setSocket(ServerConfig &server);

#endif
