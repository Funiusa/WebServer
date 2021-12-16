#include "Response.hpp"
#include <iostream>
#include <fstream>
#include <string>


void Response::getHtmlData(const std::string& uri) {
	std::ifstream out;
	std::string tmp = "/home/denis/Documents/Documents/21school/webserver/www/" + uri;

	out.open(tmp.c_str());
	if (!out.is_open()) {
		tmp = "/home/denis/Documents/Documents/21school/webserver/www/404.html";
		out.open(tmp.c_str());
		_createResponse << "HTTP/1.1 404 Not Found\r\n";
	} else
		_createResponse << "HTTP/1.1 200 OK\r\n";
	if (out.is_open()) {
		while (getline(out, tmp)) {
			tmp += "\n";
			_htmlData += tmp;
		}
	}
	out.close();
	formingResponse();
}

Response::Response() {
}

Response::~Response() {

}

void Response::formingResponse() {
	_createResponse << "version: HTTP/1.1\r\n"
			<< "server: NONGINX"
			<< "content-language: en-us, ru"
			<< "content-type: text/html; charset=utf-8\r\n"
			<< "content-length: "
			<< _htmlData.length()
			<< "\r\n\r\n"
			<< _htmlData;
	this->_response = _createResponse.str();
}

std::string Response::getResponse() {
	return _response;
}

/*****************************************************/
/* Echo the data back to the client                  */
/*****************************************************/
int Response::sandResponse(int fd) {

	this->_rc = (int)write(fd, this->_response.c_str(), this->_response.size());
	if (this->_rc < 0)
	{
		this->_closeConnection = TRUE;
		std::cout << "write failed" << std::endl;
		return (1);
	}
	return (0);
}
