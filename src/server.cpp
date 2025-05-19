#include "config.hpp"
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"

/*
	setup the socket and binding without the check yet
*/
int	setSocket(ServerConfig &server)
{
	int serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD < 0) {
		std::cerr << "Error: socket creation failed" << std::endl;
		return -1;
	}

	int opt = 1;
	if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "Error: setsockopt failed" << std::endl;
		return (close(serverFD), -1);
	}

	if (fcntl(serverFD, F_SETFL, O_NONBLOCK) < 0){
		std::cerr << "Error: setting non-blocking failed" << std::endl;
		return (close (serverFD), -1);
	}

	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(server.listenPort);
	addr.sin_addr.s_addr = inet_addr(server.listenIP.c_str());

	if (bind(serverFD, (sockaddr*)&addr, sizeof(addr)) < 0){
		std::cerr << "Error: bind" << std::endl;
		return (close (serverFD), -1);
	}

	if	(listen(serverFD, 10) < 0){
		std::cerr << "Error: listen" << std::endl;
		return (close (serverFD), -1);
	}
	std::cout << "Listening " << server.listenIP << " on port " << server.listenPort << std::endl;
	return serverFD;
}

/*
	handle connection using the poll and it handle when the client connect
	call the request parsing
	depend on result of request parse then server the correct webpage
*/
int	Server::connectionHandle(ServerConfig &server)
{
	bool isServer = false;
	while (true){
		int pollCount = poll(fds.data(), fds.size(), -1);
		if (pollCount < 0)
			continue;

		for (std::vector<struct pollfd>::size_type i = 0; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				int fd = fds[i].fd;
				isServer = false;
				for (int i = 0; i < fds.size(); i++){
					if (fd == serverFds[i]){
						isServer = true;
						break;
					}
				}

				if (isServer){
					this->clientHandle(fd);
				}
				else {
					char	buffer[4096];
					ssize_t	bytesRead = recv(fd, buffer, sizeof(buffer) + 1, 0);
					if (bytesRead > 0){
						buffer[bytesRead] = 0;
						std::cout << "[Request from client]\n" << buffer << "\n";
						HttpRequest req = parseHttpRequest(buffer);
						std::string full_response = handleRequest(req, server);
						send(fd, full_response.c_str(), full_response.length(), 0);
					}
					else{
						std::cout << "[Client disconnected]\n";
						close(fd);
						fds.erase(fds.begin() + i);
						--i;
					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}

void	Server::setServerFd(Config conf)
{
	for (int i = 0; i < conf.getServer().size(); i++){
		int serverFD = setSocket(conf.getServer()[i]);
		if (serverFD < 0)
			continue;
		serverFds.push_back(serverFD);
		struct pollfd pfd;
		pfd.fd = serverFD;
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
	}
}

void	Server::clientHandle(int fd){
	int clientFD = accept(fd, NULL, NULL);
	if (clientFD > fd){
		struct pollfd newPfd;
		newPfd.fd = clientFD;
		newPfd.events = POLLIN;
		newPfd.revents = 0;
		fds.push_back(newPfd);
		std::cout << "[Client connected]\n" << std::endl;
	}
}
