
#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP


#include <iostream>
#include <map>
#include <sstream>
#include <dirent.h>

#include "server.hpp"
#include "Request.hpp"


class Request {
private:
	std::string _type;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _mapRequest;
	char _buffer[1024 * 32];
	std::string _request;
	std::string _location;
public:
	Request();
	~Request();
	int receiveDate(int fd);
	void readRequest();
//	void readRequest(char *);
	std::string get_type();
	std::string get_uri();
	std::string getLocation();
	void setLocation(const std::string&);
};


#endif //WEBSERVER_REQUEST_HPP
