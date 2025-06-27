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

int Server::connectionHandle() {
	while (true) {
		int pollCount = poll(fds.data(), fds.size(), -1);
		if (pollCount < 0)
			continue;

		for (size_t i = 0; i < fds.size(); ++i) {
			int fd = fds[i].fd;

			// Handle read (POLLIN)
			if (fds[i].revents & POLLIN) {
				if (this->isServerCheck(fd)) {
					this->clientHandle(fd); // Accept new client
				} else {
					char buffer[4096];
					ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);
					if (bytesRead > 0) {
						buffers[fd].append(buffer, bytesRead);
						if (!headerParsed[fd])
							this->headerParser(fd);
						if (buffers[fd].size() >= expectedBodyLen[fd]) {
							prepareResponse(fd); // Prepares and queues response
							fds[i].events |= POLLOUT; // Mark for write
						}
					} else {
						this->clientDisconnect(fd, i);
						--i;
					}
				}
			}

			// Handle write (POLLOUT)
			else if (fds[i].revents & POLLOUT) {
				if (responseQueue.find(fd) != responseQueue.end()) {
					const std::string& response = responseQueue[fd];
					ssize_t bytesSent = send(fd, response.c_str(), response.length(), 0);
					if (bytesSent <= 0) {
						this->clientDisconnect(fd, i);
						--i;
						continue;
					}
					// Reset state after response sent
					responseQueue.erase(fd);
					buffers.erase(fd); // was: buffers[fd].clear(); ← this is safer
					headerParsed.erase(fd);
					expectedBodyLen.erase(fd);
					parsedRequest.erase(fd);
					fds[i].events &= ~POLLOUT;
				}
			}
		}
	}
	return EXIT_SUCCESS;
}


void	Server::prepareResponse(int clientFD) {
	std::ostringstream oss;
	oss << "Preparing response for client: " << clientFD << std::endl;
	logPrint("INFO", oss.str());

	parsedRequest[clientFD].body = buffers[clientFD].substr(0, expectedBodyLen[clientFD]);
	ServerConfig server = clientFdToConfig[clientFD];
	std::string hostname = parsedRequest[clientFD].headers["Host"];
	size_t colon = hostname.find(':');
	if (colon != std::string::npos)
		hostname = trim(hostname.substr(0, colon));

	if (server.serverName != "" && server.serverName != hostname){
		std::cout << "Host: " << hostname << " | Server: " << server.serverName << std::endl;
		responseQueue[clientFD] = makeResponse(server, 400, "Bad request", "");
		return;
	}

	std::string fullResponse = handleRequest(parsedRequest[clientFD], server);
	responseQueue[clientFD] = fullResponse;
}



void	Server::setServerFd(Config conf){
	std::vector<std::string> ipPortlst;

	for (size_t i = 0; i < conf.getServer().size(); i++){
		int serverFD;
		std::ostringstream oss;
		oss << conf.getServer()[i].listenIP << ":" << conf.getServer()[i].listenPort;

		std::vector<std::string>::iterator it = std::find(ipPortlst.begin(), ipPortlst.end(), oss.str());
		if (it == ipPortlst.end()) {
			serverFD = setSocket(conf.getServer()[i]);
			ipPortlst.push_back(oss.str());
		}

		if (serverFD < 0)
			continue;

		serverFds.push_back(serverFD);

		struct pollfd pfd;
		pfd.fd = serverFD;
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);

		serverFdToConfig[serverFD] = conf.getServer()[i];
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
