#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP
#include <iostream>
#include <sstream>
#include "server.hpp"


class Response: public Server{
private:
	std::stringstream _createResponse;
	std::string _response;
	std::string _htmlData;
	void formingResponse();
public:
	Response();
	~Response();
	void getHtmlData(const std::string& uri);
	int sandResponse(int fd);
	std::string getResponse();

};


#endif //WEBSERVER_RESPONSE_HPP
