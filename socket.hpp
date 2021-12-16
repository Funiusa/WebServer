#ifndef WEBSERVER_SOCKET_HPP
#define WEBSERVER_SOCKET_HPP

#include "server.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#define MAX_CONNECTIONS 1000

class Socket {
private:
	int _sockSD;
	int _optVal;
	struct sockaddr_in _addr;

public:
	Socket();
	explicit Socket(int port);
	~Socket() {};
	void createSocketDescriptor();
	void setSockOpt();
	void nonBlockSocket();
	void socketBind(int serverPort);
	void socketListener();

	int getSockDescriptor();

};


#endif //WEBSERVER_SOCKET_HPP
