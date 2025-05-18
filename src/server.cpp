#include "config.hpp"
#include "server.hpp"
#include "request.hpp"

/*
	setup the socket and binding without the check yet
*/
int	setSocket(ServerConfig &server)
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(server.listen_port);
	addr.sin_addr.s_addr = inet_addr(server.listen_ip.c_str());

	if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
		std::cerr << "Error: bind" << std::endl;
		return (close (server_fd), -1);
	}

	if	(listen(server_fd, 10) < 0){
		std::cerr << "Error: listen" << std::endl;
		return (close (server_fd), -1);
	}
	std::cout << "Listening " << server.listen_ip << " on port " << server.listen_port << std::endl;
	return server_fd;
}

/*
	handle connection and call the request parsing
	depend on result of request parse then server the correct webpage
*/
int	connectionHandle(int server_fd)
{
	char	buffer[4096];
	std::cout << "[Listening]" << std::endl;
	while (true){
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0) continue;
		std::cout << "[Client Connected]" << std::endl;

		ssize_t	bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read > 0)
		{
			std::cout << "[Request] \n" << buffer << std::endl;

			HttpRequest req = parseHttpRequest(buffer);

			//static text for now need a function to parse the requrest and then serve the correct gtml page
			std::string body = "<html><body><h1>Hello from C++ Web Server</h1></body></html>";
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << body.length() << "\r\n";
            response << "Connection: close\r\n";
            response << "\r\n";
            response << body;

            std::string full_response = response.str();
			send(client_fd, full_response.c_str(), full_response.length(), 0);
		}
		close(client_fd);
	}
	return EXIT_SUCCESS;
}
