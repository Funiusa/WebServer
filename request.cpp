
#include "request.hpp"
#include <vector>

/*****************************************************/
/* Receive data on this connection until the         */
/* read fails with EWOULDBLOCK. If any other         */
/* failure occurs, we will close the                 */
/* connection.                                       */
/*****************************************************/
int Request::receiveDate(int fd) {
	this->_rc = (int)read(fd, this->_buffer, sizeof(this->_buffer) - 1);
	if (this->_rc < 0) {
		if (errno != EWOULDBLOCK) {
			this->_closeConnection = TRUE;
			std::cout << "read failed" << std::endl;
		}
		return (1);
	}
	this->_buffer[this->_rc] = '\0';

	/*****************************************************/
	/* Check to see if the connection has been           */
	/* closed by the client                              */
	/*****************************************************/
	if (this->_rc == 0) {
		this->_closeConnection = TRUE;
		std::cout << "Connection closed" << std::endl;
		return (1);
	}
	return (0);
}


void Request::readRequest() {

	std::istringstream iss(this->_buffer);
	iss >> this->_type;
	iss >> this->_uri;
}
void Request::readRequest(char *buffer) {

	std::istringstream iss(buffer);
	iss >> this->_type;
	iss >> this->_uri;
}

std::string Request::get_type() {
	return this->_type;
}

std::string Request::get_uri() {
	if (this->_uri == "/" || this->_uri == "/index" || this->_uri == "/index.htm")
		this->_uri = "index.html";
	else if (this->_uri == "/blog" || this->_uri == "/blog.htm")
		this->_uri = "blog.html";
	return this->_uri;
}

Request::Request(): _type(""),  _uri("/") {

}

Request::~Request() {

}
