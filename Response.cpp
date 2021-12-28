#include "Response.hpp"
#include "server.hpp"
#include <filesystem>
#include <sys/stat.h>


Response::Response() {

}

Response::~Response() {

}

void Response::getHtmlData(const std::string& uri) {
	std::ifstream out;
	std::string tmp = "/home/denis/Documents/Documents/21school/webserver/www" + uri;

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


void Response::formingResponse() {
	_createResponse << "version: HTTP/1.1\r\n"
			<< "server: NONGINX\r\n"
			<< "content-language: en-us, ru\r\n"
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
int Response::sendResponse(int fd) {
	int rc = 0;
	rc = (int)send(fd, this->_response.c_str(), this->_response.size(), MSG_NOSIGNAL);

	if (rc < 0) {
		close(fd);
		std::cout << "Send failed" << std::endl;
		return (1);
	}
	return (0);
}

int Response::createAutoindexResponse(const std::string &uri) {
	DIR *dir;
	struct dirent *ent;
	struct stat s;

	if ((dir = opendir(this->_location.append(uri).c_str())) != nullptr)
	{
		std::vector<std::string> autoindex;
		while ((ent = readdir(dir)) != nullptr) {
			autoindex.push_back(ent->d_name);
//			std::cout << open(ent->d_name, S_IFDIR) << std::endl;
		}
		closedir(dir);
		_htmlData = "<!DOCTYPE html>\n<html>\n"
					"<head>\n"
					"<title>Index of " + uri + "</title>\n</head>\n"
					"<body bgcolor='white'>\n"
					"<h1 style='font-size:50px;'>Index of " + uri + "</h1>\n<hr>\n"
					"<pre>";

		for (int i = 0; i < (int)autoindex.size(); i++) {
			if (lstat((this->_location + "/" + autoindex.at(i)).c_str(), &s) == 0) {
				if (autoindex.at(i) == ".")
					continue;
				if (S_ISDIR(s.st_mode)) {
					autoindex.at(i).append("/");
					_htmlData.append("\n<a href='");
					_htmlData.append(autoindex.at(i));
					_htmlData.append("'>");
					_htmlData.append(autoindex.at(i));
					_htmlData.append("</a>\n");
				}
				else {
					_htmlData.append("\n<a href='data:application/xml;charset=utf-8'");
					_htmlData.append(" download='");
					_htmlData.append(autoindex.at(i));
					_htmlData.append("'>");
					_htmlData.append(autoindex.at(i));
					_htmlData.append("</a>\n");
				}
			}


		}

		_htmlData.append("\n</pre>\n<hr>\n</body>\n</html>");
		_createResponse << "HTTP/1.1 200 OK\r\n"
						<< "version: HTTP/1.1\r\n"
						<< "server: NONGINX\r\n"
						<< "content-language: en-us, ru\r\n"
						<< "content-type: text/html; charset=utf-8\r\n"
						<< "content-length: "
						<< _htmlData.length()
						<< "\r\n\r\n"
						<< _htmlData;
		this->_response = _createResponse.str();
	}
	else {
		return 1;
	}
	return 0;
}

std::string Response::getLocation() {
	return this->_location;
}

void Response::setLocation(const std::string &location) {
	this->_location = location;
}



