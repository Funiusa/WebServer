#include "Response.hpp"

Response::Response() {
}

Response::~Response() {
}

Response &Response::operator=(const Response &src) {
	_response = src._response;
	_path = src._path;
	_code = src._code;
	return (*this);
}

std::map<std::string, void (Response::*)(Request &, RequestConfig &)> Response::initMethods() {
	std::map<std::string, void (Response::*)(Request &, RequestConfig &)> map;
	map["GET"] = &Response::getMethod;
	map["PUT"] = &Response::putMethod;
	map["HEAD"] = &Response::headMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;
	return map;
}

std::map<std::string, void (Response::*)(Request &, RequestConfig &)> Response::_method = Response::initMethods();

void Response::answerToClient(Request &request, RequestConfig &requestConf) {
	_hostPort.host = requestConf.getHostPort().host;
	_hostPort.port = requestConf.getHostPort().port;
	_isAutoIndex = requestConf.getAutoIndex();
	_errorMap = requestConf.getErrorPage();
	_path = requestConf.getPath();
	_code = request.getRet();
	unsigned int currentClientBodySize = request.getBody().size();
	unsigned int configClientBodySize = requestConf.getClientBodyBufferSize();

	if (requestConf.getAllowedMethods().find(request.getMethod())
		== requestConf.getAllowedMethods().end()) {
		_code = 405;
	} else if (configClientBodySize < currentClientBodySize)
		_code = 413;
	if (_code == 405 || _code == 413) {
		request.setMethod("GET");
	}
	(this->*Response::_method[request.getMethod()])(request, requestConf);
}

void Response::getMethod(Request &request, RequestConfig &requestConf) {
	ResponseHeader head;

	if (!requestConf.getCgiPass().empty() && _code == 200) {

		CgiHandler cgi(request, requestConf);
		size_t i = 0;
		size_t j = _response.size() - 2;

		_response = cgi.executeCgi(requestConf.getCgiPass());

		while (_response.find("\r\n\r\n", i) != std::string::npos
			   || _response.find("\r\n", i) == i) {
			std::string str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = (int) strtol(str.substr(8, 3).c_str(), NULL, 10);
			else if (str.find("Content-type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;
		_response = _response.substr(i, j - i);
	} else if (_code == 200)
		_code = readContent();
	else
		_response = this->readHtml(_errorMap[_code]);
	if (_code == 500)
		_response = this->readHtml(_errorMap[_code]);

	_response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(),
							   requestConf.getLang()) + "\r\n" + _response;
}

void Response::headMethod(Request &request, RequestConfig &requestConf) {
	(void) request;
	ResponseHeader head;

	_code = readContent();
	_response = head.getHeader(_response.size(), _path, _code, _type,
							   requestConf.getContentLocation(),
							   requestConf.getLang()) + "\r\n";
}

void Response::postMethod(Request &request, RequestConfig &requestConf) {
	ResponseHeader head;

	if (!requestConf.getCgiPass().empty()) {
		size_t i = 0, j = _response.size() - 2;
		CgiHandler cgi(request, requestConf);
		_response = cgi.executeCgi(requestConf.getCgiPass());

		while (_response.find("\r\n\r\n", i) != std::string::npos
			   || _response.find("\r\n", i) == i) {
			std::string str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = (int) std::strtol(str.substr(8, 3).c_str(), NULL, 10);
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j) {
			j -= 2;
		}
		_response = _response.substr(i, j - i);
	} else {

		_code = writeContent(request.getBody());
//		_code = 204;
		_response = this->readHtml(_errorMap[_code]);
	}
	if (_code == 500) {
		_response = this->readHtml(_errorMap[_code]);
	}
	_response = head.getHeader(_response.size(), _path, _code, _type,
							   requestConf.getContentLocation(),
							   requestConf.getLang()) + "\r\n" + _response;
}


void Response::putMethod(Request &request, RequestConfig &requestConf) {
	ResponseHeader head;
	_response = "";
	_code = writeContent(request.getBody());

	if (_code != 201 && _code != 204)
		_response = this->readHtml(_errorMap[_code]);
	_response = head.getHeader(_response.size(),
							   _path, _code, _type,
							   requestConf.getContentLocation(),
							   requestConf.getLang()) + "\r\n" + _response;
}

// Find and remove file
void Response::deleteMethod(Request &request, RequestConfig &requestConf) {
	ResponseHeader head;
	_response = "";

	if (pathIsFile(_path) == 1) {
		_code = remove(_path.c_str()) == 0 ? 200 : 403;
	} else _code = 404;
	if (_code == 403 || _code == 404) {
		_response = this->readHtml(_errorMap[_code]);
	}
	_response = head.getHeader(_response.size(),
							   _path, _code, _type,
							   requestConf.getContentLocation(),
							   requestConf.getLang()) + "\r\n" + _response;
	(void) request;
}

int Response::readContent() {
	std::ifstream file;
	std::stringstream buffer;

	_response = "";
	if (pathIsFile(_path) == 1) {
		file.open(_path.c_str(), std::ifstream::in);
		if (!file.is_open()) {
			_response = this->readHtml(_errorMap[403]);
			return (403);
		}
		buffer << file.rdbuf();
		_response = buffer.str();
		file.close();
	} else if (_isAutoIndex && !pathIsFile(this->_path)) {
		buffer << AutoIndexGenerator::getPage(_path);
		_response = buffer.str();
		_type = "text/html";
	}
	if (_response.empty()) {
		_response = this->readHtml(_errorMap[404]);
		return (404);
	}
	return (200);
}

int Response::writeContent(const std::string &content) {
	std::ofstream file;

	if (pathIsFile(_path)) {
		file.open(_path.c_str());
		file << content;
		file.close();
		return (204);
	} else {
		file.open(_path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open())
			return (403);
		file << content;
		file.close();
		return (201);
	}
}

std::string Response::readHtml(const std::string &path) {
	std::ofstream file;
	std::stringstream buffer;

	_type = "text/html";
	if (pathIsFile(path)) {
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open()) {
			buffer << file.rdbuf();
			file.close();
			return (buffer.str());
		}
	}
	return "<!DOCTYPE html>\n"
		   "<html>\n""\n"
		   "<head>\n"
		   "    <title>404 Page Not Found</title>\n"
		   "    <link rel=\"icon\" type=\"image/x-icon\" href=\"images/favicon.ico\">\n"
		   "</head>\n""\n"
		   "<body bgcolor=\"white\">\n"
		   "<center>\n"
		   "    <h1>404 Page Not Found</h1>\n"
		   "</center>\n""<hr>\n"
		   "<center>Nonginx / 0.5</center>\n"
		   "\n""</body>\n""\n""</html>";
}

std::string Response::getResponse() {
	return _response;
}


Response::Response(const Response &src) {
	this->operator=(src);
}
