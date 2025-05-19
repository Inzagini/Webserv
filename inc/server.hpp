#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "config.hpp"

int	setSocket(ServerConfig &server);
int	connectionHandle(ServerConfig &server);

#endif
