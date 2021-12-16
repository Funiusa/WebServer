#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP

#include <iostream>
#include <sstream>
#include "server.hpp"


class Request: public Server {
private:
	std::string _type;
	std::string _uri;
	char _buffer[1024];
public:
	Request();
	~Request();
	int receiveDate(int fd);
	void readRequest();
	void readRequest(char *);
	std::string get_type();
	std::string get_uri();
};


#endif //WEBSERVER_REQUEST_HPP
