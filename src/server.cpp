/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   server.cpp										 :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: quannguy <quannguy@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/05/21 12:02:55 by quannguy		  #+#	#+#			 */
/*   Updated: 2025/05/21 12:03:27 by quannguy		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

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
int	Server::connectionHandle(ServerConfig &server){
	while (true) {
		int pollCount = poll(fds.data(), fds.size(), -1);
		if (pollCount < 0)
			continue;

		for (std::vector<struct pollfd>::size_type i = 0; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				int fd = fds[i].fd;
				bool isServer = false;
				for (size_t j = 0; j < serverFds.size(); ++j) {
					if (fd == serverFds[j]) {
						isServer = true;
						break;
					}
				}

				if (isServer) {
					this->clientHandle(fd);
				}
				else {
					char buffer[4096];
					ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);
					if (bytesRead > 0) {
						buffers[fd].append(buffer, bytesRead);
						if (!headerParsed[fd])
							this->headerParser(fd);

						// Header parsed, wait for full body
						if (buffers[fd].size() >= expectedBodyLen[fd]) {
							this->ReqResHandle(fd, server);
						}
					}
					else {
						std::cout << "[Client disconnected]\n";
						close(fd);
						fds.erase(fds.begin() + i);
						buffers.erase(fd);
						headerParsed.erase(fd);
						expectedBodyLen.erase(fd);
						parsedRequest.erase(fd);
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

/*
	parse the header from the request and assign data to struct
	check for content if exist
	then remove header from buffer
*/
void	Server::headerParser(int fd){
	size_t headerEnd = buffers[fd].find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		std::string headerPart = buffers[fd].substr(0, headerEnd + 4);
		parsedRequest[fd] = parseHttpRequest(headerPart.c_str());
		headerParsed[fd] = true;

		std::map<std::string, std::string>::iterator it = parsedRequest[fd].headers.find("Content-Length");
		if (it != parsedRequest[fd].headers.end())
			expectedBodyLen[fd] = std::atoi(it->second.c_str());
		else
			expectedBodyLen[fd] = 0;

		buffers[fd].erase(0, headerEnd + 4); //remove header from buffer
	}
}

void	Server::ReqResHandle(int fd, ServerConfig &server){
	parsedRequest[fd].body = buffers[fd].substr(0, expectedBodyLen[fd]);
	std::string full_response = handleRequest(parsedRequest[fd], server);
	send(fd, full_response.c_str(), full_response.length(), 0);
	buffers[fd].erase(0, expectedBodyLen[fd]);
	headerParsed[fd] = false;
	expectedBodyLen[fd] = 0;
	parsedRequest.erase(fd);
}
