#include "config.hpp"
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"

/*
	setup the socket and binding without the check yet
*/
int	setSocket(ServerConfig &server)
{
	std::ostringstream msg;

	int serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD < 0) {
		msg << "Error: socket creation failed - " << strerror(errno) << std::endl;
		logPrint("ERROR", msg.str());
		return -1;
	}

	int opt = 1;
	if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		msg << "Error: setsockopt failed - " << strerror(errno) << std::endl;
		logPrint("ERROR", msg.str());
		return (close(serverFD), -1);
	}

	if (fcntl(serverFD, F_SETFL, O_NONBLOCK) < 0){
		msg << "Error: setting non-blocking failed - " << strerror(errno) << std::endl;
		logPrint("ERROR", msg.str());
		return (close (serverFD), -1);
	}

	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(server.listenPort);
	addr.sin_addr.s_addr = inet_addr(server.listenIP.c_str());

	if (bind(serverFD, (sockaddr*)&addr, sizeof(addr)) < 0){
		msg << "Error: bind - " << strerror(errno) << std::endl;
		logPrint("ERROR", msg.str());
		return (close (serverFD), -1);
	}

	if	(listen(serverFD, 10) < 0){
		msg << "Error: listen - " << strerror(errno) << std::endl;
		logPrint("ERROR", msg.str());
		return (close (serverFD), -1);
	}

	msg << "Listening " << server.listenIP << " on port " << server.listenPort << std::endl;
	logPrint("INFO", msg.str());
	return serverFD;
}

/*
	handle connection using the poll and it handle when the client connect
	call the request parsing
	depend on result of request parse then server the correct webpage
*/
int	Server::connectionHandle(){
	while (true) {
		int	pollCount = poll(fds.data(), fds.size(), -1);
		if (pollCount < 0)
			continue;

		for (std::vector<struct pollfd>::size_type i = 0; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				int	fd = fds[i].fd;
				if (this->isServerCheck(fd))
					this->clientHandle(fd);
				else {
					char	buffer[4096];
					ssize_t	bytesRead = recv(fd, buffer, sizeof(buffer), 0);
					if (bytesRead > 0) {
						buffers[fd].append(buffer, bytesRead);
						if (!headerParsed[fd])
							this->headerParser(fd);
						if (buffers[fd].size() >= expectedBodyLen[fd])
							if (this->ReqRespHandle(fd) < 1)
								this->clientDisconnect(fd, i);
					}
					else {
						this->clientDisconnect(fd, i);
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
	for (size_t i = 0; i < conf.getServer().size(); i++){
		int serverFD = setSocket(conf.getServer()[i]);
		if (serverFD < 0)
			continue;
		serverFds.push_back(serverFD);
		serverFdToConfig[serverFD] = conf.getServer()[i];
		struct pollfd pfd;
		pfd.fd = serverFD;
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
	}
}

void	Server::clientHandle(int serverFd){
	int	clientFD = accept(serverFd, NULL, NULL);
	if (clientFD > serverFd){
		struct pollfd newPfd;
		newPfd.fd = clientFD;
		newPfd.events = POLLIN;
		newPfd.revents = 0;
		fds.push_back(newPfd);
		clientFdToConfig[clientFD] = serverFdToConfig[serverFd];
		std::ostringstream msg;
		msg << "Client: " << clientFD << " connected to server "
			<< serverFdToConfig[serverFd].listenIP << ":"
			<< serverFdToConfig[serverFd].listenPort << std::endl;
		logPrint("INFO", msg.str());
	}
}

/*
	parse the header from the request and assign data to struct
	check for content if exist
	then remove header from buffer
*/
void	Server::headerParser(int clientFd){
	size_t	headerEnd = buffers[clientFd].find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		std::string headerPart = buffers[clientFd].substr(0, headerEnd + 4);
		parsedRequest[clientFd] = parseHttpRequest(headerPart.c_str());
		headerParsed[clientFd] = true;
		std::map<std::string, std::string>::iterator it = parsedRequest[clientFd].headers.find("Content-Length");
		if (it != parsedRequest[clientFd].headers.end())
			expectedBodyLen[clientFd] = std::atoi(it->second.c_str());
		else
			expectedBodyLen[clientFd] = 0;
		buffers[clientFd].erase(0, headerEnd + 4); //remove header from buffer
	}
}

/*
	function is called when got the full request
*/
size_t	Server::ReqRespHandle(int clientFD){
	std::ostringstream msg;
	msg << "Received request from client: " << clientFD << std::endl;
	logPrint("INFO", msg.str());
	parsedRequest[clientFD].body = buffers[clientFD].substr(0, expectedBodyLen[clientFD]);

	ServerConfig server = clientFdToConfig[clientFD];
	std::string fullResponse = handleRequest(parsedRequest[clientFD], server);
	size_t bytes = send(clientFD, fullResponse.c_str(), fullResponse.length(), 0);

	buffers[clientFD].erase(0, expectedBodyLen[clientFD]);
	headerParsed[clientFD] = false;
	expectedBodyLen[clientFD] = 0;
	parsedRequest.erase(clientFD);
	return bytes;
}

void	Server::clientDisconnect(int clientFd, int i){
	std::ostringstream msg;
	msg << "Client: " << clientFd << " disconneted" << std::endl;
	logPrint("INFO", msg.str());
	fds.erase(fds.begin() + i);
	buffers.erase(clientFd);
	headerParsed.erase(clientFd);
	expectedBodyLen.erase(clientFd);
	parsedRequest.erase(clientFd);
	close(clientFd);
}

bool	Server::isServerCheck(int fd){
	for (size_t j = 0; j < serverFds.size(); ++j) {
		if (fd == serverFds[j])
			return true;
	}
	return false;
}

Server::~Server(){
	for (size_t i = 0; i < serverFds.size(); i++){
		close(serverFds[i]);
	}
}
