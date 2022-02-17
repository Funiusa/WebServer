#include "Server.hpp"

/*************************************************************/
/* Create an AF_INET stream socket to receive incoming      */
/* connections on                                            */
/*************************************************************/
int Server::socketDescriptor() {
	_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_fd <= 0) {
		std::cerr << RED << "Creation socket description is failed" << RESET << std::endl;
		return -1;
	} else
		std::cout << BLUE << "Socket " << _fd << " create";
	return 0;
}

/*************************************************************/
/* Allow socket descriptor to be reuseable                   */
/*************************************************************/
int Server::setSockOpt() {
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR,
				   &_optVal, sizeof(_optVal)) < 0) {
		closeSockFD(_fd);
		std::cerr << RED << "Socket options failed" << RESET << std::endl;
		return (-1);
	}
	return 0;
}

/*************************************************************/
/* Set socket to be nonblocking. All of the sockets for      */
/* the incoming connections will also be nonblocking since   */
/* they will inherit that state from the listening socket.   */
/*************************************************************/
int Server::nonBlockSocket() {

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
		closeSockFD(_fd);
		std::cerr << RED << "fcntl() failed" << RESET << std::endl;
		return -1;
	}
	return 0;
}

/*************************************************************/
/* Bind the socket                                           */
/*************************************************************/
int Server::socketBind() {
	memset(&_addr.sin_zero, 0, sizeof(_addr.sin_zero));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(_listen.host);
	_addr.sin_port = htons(_listen.port);
	memcpy(&this->_addr.sin_addr, &this->_addr.sin_addr.s_addr, sizeof(this->_addr.sin_addr.s_addr));
	if (bind(_fd, (struct sockaddr *) &_addr, sizeof(_addr)) < 0) {
		closeSockFD(_fd);
		std::cerr << RED << "Bind socket failed on port: " << _listen.port << RESET << std::endl;
		return -1;
	} else
		std::cout << BLUE << ", bind";
	return 0;
}

/*************************************************************/
/* Set the listen    		                                 */
/*************************************************************/
int Server::socketListener() {
	if (listen(_fd, MAX_CONNECTIONS) < 0) {
		closeSockFD(_fd);
		std::cerr << RED << "Listen socket failed" << RESET << std::endl;
		return -1;
	}
	std::cout << BLUE << " and listen on host: " << _listen.host
			  << " and port: " << _listen.port << RESET << std::endl;
	return 0;
}
