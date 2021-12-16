#include "socket.hpp"
#include "server.hpp"

#define SERVER_PORT  12345

#define TRUE             1
#define FALSE            0


int main ()
{

	int    listen_sd = -1;

	Socket sock(SERVER_PORT);
	listen_sd = sock.getSockDescriptor();

	Server server(SERVER_PORT);
	server.setListenSd(listen_sd);
	server.pollInit();
	server.start();

	return 0;
}


