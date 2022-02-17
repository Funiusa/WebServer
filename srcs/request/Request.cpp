#include "Request.hpp"

vectorOfStrings Request::initMethods() {
	vectorOfStrings methods;
	methods.push_back("GET");
	methods.push_back("HEAD");
	methods.push_back("POST");
	methods.push_back("PUT");
	methods.push_back("DELETE");
	return methods;
}

std::vector<std::string> Request::methods = Request::initMethods();

Request::Request(const std::string &str) :
		_ret(200), _port(8000),
		_bodyRequest(""), _path(""), _query(""),
		_method(""), _version(""), _raw(str) {
	this->resetHeaders();
	this->_env_for_cgi.clear();
	this->parseRequestStr(str);
	if (this->_ret != 200)
		std::cerr << RED << "Parse error: " << this->_ret << RESET << std::endl;
}

Request::~Request() {
}

Request &Request::operator=(const Request &obj) {
	this->_headers = obj.getHeaders();
	this->_env_for_cgi = obj.getEnv();
	this->_method = obj.getMethod();
	this->_version = obj.getVersion();
	this->_ret = obj.getRet();
	this->_bodyRequest = obj.getBody();
	this->_port = obj.getPort();
	this->_path = obj.getPath();
	return *this;
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return this->_headers;
}

const std::map<std::string, std::string> &Request::getEnv() const {
	return this->_env_for_cgi;
}

const std::string &Request::getMethod() const {
	return this->_method;
}

const std::string &Request::getVersion() const {
	return this->_version;
}

int Request::getRet() const {
	return this->_ret;
}

const std::string &Request::getBody() const {
	return this->_bodyRequest;
}

int Request::getPort() const {
	return this->_port;
}

const std::string &Request::getPath() const {
	return this->_path;
}

const std::string &Request::getQuery() const {
	return this->_query;
}

std::ostream &operator<<(std::ostream &os, const Request &re) {
	std::map<std::string, std::string>::const_iterator it;

	os << "Method: " << re.getMethod() << " |\tHTTP version: "
	   << re.getVersion() << '\n'
	   << "Port: " << re.getPort() << '\n'
	   << "Path: " << re.getPath() << '\n';
	for (it = re.getHeaders().begin(); it != re.getHeaders().end(); it++)
		os << it->first << ": " << it->second << '\n';
	os << '\n' << "Request body:\n" << re.getBody() << '\n';
	return os;
}

void Request::resetHeaders() {
	this->_headers.clear();
	this->_headers["Accept-Charsets"] = "";
	this->_headers["Accept-Language"] = "";
	this->_headers["Allow"] = "";
	this->_headers["Auth-Scheme"] = "";
	this->_headers["Authorization"] = "";
	this->_headers["Content-Language"] = "";
	this->_headers["Content-Length"] = "";

	this->_headers["Content-Location"] = "";
	this->_headers["Content-Type"] = "";
	this->_headers["Date"] = "";
	this->_headers["Host"] = "";

	this->_headers["Last-Modified"] = "";
	this->_headers["Location"] = "";
	this->_headers["Referer"] = "";
	this->_headers["Retry-After"] = "";
	this->_headers["Server"] = "";
	this->_headers["Transfer-Encoding"] = "";
	this->_headers["User-Agent"] = "";
	this->_headers["Www-Authenticate"] = "";
	this->_headers["Connection"] = "Keep-Alive";
}

int Request::readFirstLine(const std::string &str) {
	size_t i;
	std::string line;

	i = str.find_first_of('\n');
	line = str.substr(0, i);
	i = line.find_first_of(' ');

	if (i == std::string::npos) {
		std::cerr << RED << "Read first line no space after method" << RESET << std::endl;
		this->_ret = 400;
		return this->_ret;
	}
	this->_method.assign(line, 0, i);
	return this->readPath(line, i);
}

int Request::readPath(const std::string &line, size_t i) {
	size_t j;

	if ((j = line.find_first_not_of(' ', i)) == std::string::npos) {
		std::cerr << RED << "No PATH / HTTP version" << RESET << std::endl;
		this->_ret = 400;
		return this->_ret;
	}
	if ((i = line.find_first_of(' ', j)) == std::string::npos) {
		std::cerr << RED << "No HTTP version" << RESET << std::endl;
		this->_ret = 400;
		return this->_ret;
	}
	this->_path.assign(line, j, i - j);
	this->_path = removeSlashes(_path);
	if (this->_path[_path.size() - 1] != '/' && pathIsFile(this->_path) != 0)
		this->_path.append("/");
	return this->readVersion(line, i);
}

int Request::readVersion(const std::string &line, size_t i) {
	if ((i = line.find_first_not_of(' ', i)) == std::string::npos) {
		std::cerr << RED << "No HTTP version" << RESET << std::endl;
		this->_ret = 400;
		return this->_ret;
	}
	if (line[i] == 'H' && line[i + 1] == 'T' && line[i + 2] == 'T' &&
		line[i + 3] == 'P' && line[i + 4] == '/')
		this->_version.assign(line, i + 5, 3);
	if (this->_version != "1.0" && this->_version != "1.1") {
		std::cerr << RED << "BAD HTTP VERSION (" << this->_version << ")" << RESET << std::endl;
		this->_ret = 400;
		return this->_ret;
	}
	return (this->checkMethod());
}

int Request::checkMethod() {
	for (size_t i = 0; i < this->methods.size(); i++) {
		if (this->methods[i] == this->_method)
			return this->_ret;
	}
	std::cerr << RED << "Invalid method requested" << RESET << std::endl;
	this->_ret = 405;
	return this->_ret;
}

std::string Request::nextLineInString(const std::string &str, size_t &i) {
	std::string ret;
	size_t j;

	if (i == std::string::npos)
		return "";
	j = str.find_first_of('\n', i);
	ret = str.substr(i, j - i);
	if (ret[ret.size() - 1] == '\r')
		pop(ret);
	if (j == std::string::npos) {
		i = j;
	} else {
		i = j + 1;
	}
	return ret;
}

// Parse request buffer
int Request::parseRequestStr(const std::string &str) {
	std::string key;
	std::string value;
	std::string line;
	size_t i = 0;

	this->readFirstLine(nextLineInString(str, i));
	while ((line = nextLineInString(str, i)) != "\r"
		   && !line.empty() && this->_ret != 400) {
		key = readKey(line);
		value = readValue(line);
		if (this->_headers.count(key))
			this->_headers[key] = value;
		if (key.find("Secret") != std::string::npos)
			this->_env_for_cgi[formatHeaderForCGI(key)] = value;
	}
	if (!this->_headers["Www-Authenticate"].empty())
		this->_env_for_cgi["Www-Authenticate"] = this->_headers["Www-Authenticate"];
	this->setBody(str.substr(i, std::string::npos));
	this->findQuery();
	return this->_ret;
}

void Request::findQuery() {
	size_t i = this->_path.find_first_of('?');
	if (i != std::string::npos) {
		this->_query.assign(this->_path, i + 1, std::string::npos);
		this->_path = this->_path.substr(0, i);
	}
}

std::string Request::formatHeaderForCGI(std::string &key) {
	std::transform(key.begin(), key.end(),
				   key.begin(), ::toupper);
	for (size_t i = 0; i < key.size(); i++) {
		if (key[i] == '-')
			key[i] = '_';
	}
	return "HTTP_" + key;
}

void Request::setBody(const std::string &str) {
	char strip[] = {'\n', '\r'};

	this->_bodyRequest.assign(str);
	for (int i = 0; i < 4; i++) {
		if (this->_bodyRequest.size() > 0
			&& this->_bodyRequest[this->_bodyRequest.size() - 1] == strip[i % 2])
			pop(this->_bodyRequest);
		else
			break;
	}
}


void Request::setMethod(const std::string &method) {
	this->_method = method;
}

