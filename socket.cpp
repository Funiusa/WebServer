
#include "socket.hpp"
#include "server.hpp"

/*************************************************************/
/* Create an AF_INET stream socket to receive incoming      */
/* connections on                                            */
/*************************************************************/
void Socket::createSocketDescriptor() {
	this->_sockSD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->_sockSD == 0)
		throw Server::myErrorException("Creation socket description is failed");
	else
		std::cout << "Socket create" << std::endl;
}

/*************************************************************/
/* Allow socket descriptor to be reuseable                   */
/*************************************************************/
void Socket::setSockOpt() {

	if (setsockopt(this->_sockSD, SOL_SOCKET,  SO_REUSEADDR,
					&this->_optVal, sizeof(this->_optVal)) < 0)  {
		close(this->_sockSD);
		throw Server::myErrorException("Socket options failed");
	}
}

/*************************************************************/
/* Set socket to be nonblocking. All of the sockets for      */
/* the incoming connections will also be nonblocking since   */
/* they will inherit that state from the listening socket.   */
/*************************************************************/
void Socket::nonBlockSocket() {

	if (fcntl(this->_sockSD, F_SETFL, O_NONBLOCK) < 0) {
		close(this->_sockSD);
		this->_sockSD = -1;
		throw Server::myErrorException("fcntl() failed");
	}
}

/*************************************************************/
/* Bind the socket                                           */
/*************************************************************/
void Socket::socketBind(int serverPort) {
	memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
	this->_addr.sin_family      = AF_INET;
	this->_addr.sin_addr.s_addr = INADDR_ANY;
	memcpy(&this->_addr.sin_addr, &this->_addr.sin_addr.s_addr, sizeof(this->_addr.sin_addr.s_addr));
	this->_addr.sin_port        = htons(serverPort);
	if (bind(this->_sockSD, (struct sockaddr *)&this->_addr, sizeof(this->_addr)) < 0) {
		close(this->_sockSD);
		throw Server::myErrorException("bind socket failed");
	}
	else
		std::cout << "Socket bind" << std::endl;

}

/*************************************************************/
/* Set the listen    		                                 */
/*************************************************************/
void Socket::socketListener() {
	if (listen(this->_sockSD, MAX_CONNECTIONS) < 0) {
		close(this->_sockSD);
		throw Server::myErrorException("listen socket failed");
	}

}

Socket::Socket() : _sockSD(-1), _optVal(1), _addr() {
	try {
		createSocketDescriptor();
		setSockOpt();
//		nonBlockSocket();
		socketBind(8080);
		socketListener();
	}
	catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	std::cout << "Socket listen on port: 8080" << std::endl;
}

Socket::Socket(int port) {
	try {
		createSocketDescriptor();
		setSockOpt();
		nonBlockSocket();
		socketBind(port);
		socketListener();
	}
	catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	std::cout << "Socket listen on port: " << port << std::endl;
}

int Socket::getSockDescriptor() {
	return this->_sockSD;
}







