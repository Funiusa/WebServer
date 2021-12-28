
#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP

#include "server.hpp"
#include "Response.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <filesystem>

class Server;

class Response {
private:
	std::stringstream _createResponse;
	std::string _response;
	std::string _htmlData;
	std::string _location;
	void formingResponse();
public:
	Response();
	~Response();
	void getHtmlData(const std::string& uri);
	int sendResponse(int fd);
	int createAutoindexResponse(const std::string &uri);
	void setLocation(const std::string&);
	std::string getResponse();
	std::string getLocation();

};


#endif //WEBSERVER_RESPONSE_HPP
