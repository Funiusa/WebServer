#include "Request.hpp"
#include "server.hpp"



Request::Request(): _type(""),  _uri("/") {

}

Request::~Request() {

}

/*****************************************************/
/* Receive data on this connection until the         */
/* read fails with EWOULDBLOCK. If any other         */
/* failure occurs, we will close the                 */
/* connection.                                       */
/*****************************************************/
int Request::receiveDate(int fd) {
	int rc = 0;
	rc = (int)recv(fd, this->_buffer, sizeof(this->_buffer), MSG_NOSIGNAL);
	if (rc > 0)	{
		this->_buffer[rc] = '\0';
		this->_request.append(this->_buffer);
	}
	return (rc);
}


void Request::readRequest() {

	std::istringstream iss(this->_buffer);
	iss >> this->_type;
	iss >> this->_uri;
	if (this->_uri == "/" || this->_uri == "/index" || this->_uri == "/index.htm")
		this->_uri = "/index.html";

	iss >> this->_version;


}

std::string Request::get_type() {
	return this->_type;
}

std::string Request::get_uri() {
	return this->_uri;
}

std::string Request::getLocation() {
	return this->_location;
}

void Request::setLocation(const std::string &location) {
	this->_location = location;
}

//void Request::readRequest(char *buffer) {
//
//	std::istringstream iss(buffer);
//	iss >> this->_type;
//	iss >> this->_uri;
//}

//	int pos = this->_request.find("\r\n\r\n");
//	while (this->_request.begin() != this->_request.end()) {
//		std::cout << this->_request.begin().base();
//
//		this->_mapRequest.insert(std::pair<std::string, std::string>(key, val));
//	}
//	std::map<std::string, std::string>::iterator it = this->_mapRequest.begin();
//	for (; it != this->_mapRequest.end(); ++it) {
//		std::cout << it->first << " " << it->second << std::endl;
//	}
