#include <iostream>
#include <sstream>
#include <string>
#include <cstring>     // For memset
#include <cstdlib>     // For exit()
#include <unistd.h>    // For close()
#include <netinet/in.h> // For sockaddr_in, htons, etc.
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <arpa/inet.h>

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
    std::cout << "Listening on port 8080..." << std::endl;

	char buffer[4096];
	 while (true) {
       int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        std::cout << "Client connected.\n";

        // 7. Receive request
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate string
            std::cout << "Request:\n" << buffer << std::endl;

            // 8. Build HTTP response
            std::string body = "<html><body><h1>Hello from C++ Web Server</h1></body></html>";
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << body.length() << "\r\n";
            response << "Connection: close\r\n";
            response << "\r\n";
            response << body;

            std::string full_response = response.str();

            // 9. Send response
            send(client_fd, full_response.c_str(), full_response.length(), 0);
        }

        // 10. Close client connection
        close(client_fd);
        std::cout << "Client disconnected.\n\n";
    }

    // Never reached, but clean up in case
    close(server_fd);
    return 0;
}

GET 
