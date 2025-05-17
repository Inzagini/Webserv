#include "config.hpp"
#include "server.hpp"

// int	setSocket(Config &server)
// {
// 	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

// 	sockaddr_in	addr;
// 	addr.sin_family = AF_INET;
// 	addr.sin_port = htons(server.getPort());
// 	addr.sin_port = inet_addr(server.getListenIP().c_str());
// 	std::cout << "Listening " << server.getListenIP() << " on port " << server.getPort() << std::endl;

// 	bind(server_fd, (sockaddr*)&addr, sizeof(addr));
// 	return server_fd;
// }

// int	connectionHandle(int server_fd)
// {
// 	char	buffer[4096];
// 	std::cout << "[Listening]" << std::endl;
// 	while (true){
// 		int client_fd = accept(server_fd, NULL, NULL);
// 		if (client_fd < 0) continue;
// 		std::cout << "[Client Connected]" << std::endl;

// 		ssize_t	bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
// 		if (bytes_read > 0)
// 		{
// 			std::cout << "[Request] \n" << buffer << std::endl;
// 		}
// 		close(client_fd);
// 	}
// 	return EXIT_SUCCESS;
// }
