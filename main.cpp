#include "socket.hpp"
#include "server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <vector>

#define SERVER_PORT  12345

#define TRUE             1
#define FALSE            0


int main ()
{

	int    listen_sd = -1;
	Socket sock(SERVER_PORT);
	listen_sd = sock.getSockDescriptor();


	std::string location = "/home/denis/Documents/Documents/21school/webserver/www";
	Server server(SERVER_PORT);
	server.setLocation(location);
	server.setAutoindex(1);
	server.setListenSd(listen_sd);
	server.pollInit();
	try {
		server.start();
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}


