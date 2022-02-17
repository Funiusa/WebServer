#include "RequestConfig.hpp"

RequestConfig::RequestConfig() {
}

RequestConfig::RequestConfig(ConfigServer &config, Request &request,
							 const std::string &path, const std::string &method,
							 std::string &locationName)
		:  _cgi_pass(config.getCgiPass()),
		   _client_body_buffer_size(config.getClientBodyBufferSize()),
		   _error_page(config.getErrorPage()),
		   _cgi_param(config.getCgiParam()),

		   _allowed_methods(config.getAllowedMethods()),
		   _autoindex(config.getAutoIndex()) {
	std::string ret;
	std::string root = config.getRoot();
	std::string alias = config.getAlias();

	vectorOfStrings::const_iterator it;
	vectorOfStrings conf_index = config.getIndex();
	for (it = conf_index.begin(); it != conf_index.end(); it++) {
		vectorOfStrings::const_iterator it2 = _index.begin();
		for (it2 = _index.begin(); it2 != _index.end(); it2++)
			if (*it == *it2)
				break;
		if (it2 == _index.end()) { _index.push_back(*it); }
	}
	_cgi_param = request.getEnv();
	if (locationName[0] != '*' && config.getAliasSet()) {
		ret = root + alias + path.substr(locationName.length());
		this->_contentLocation = alias + removeSlashes(path.substr(locationName.length()));
	} else {
		ret = root + path;
		this->_contentLocation = path;
	}
	this->_path = removeSlashes(ret);
	if (pathIsFile(this->_path) == 0 && method == "GET") {
		std::string indexPath = this->addIndex();
		if (!indexPath.empty()) {
			config = config.getLocationForRequest(indexPath, locationName);
			this->_cgi_pass = config.getCgiPass();
			this->_cgi_param = config.getCgiParam();
		}
	}
}

RequestConfig &RequestConfig::operator=(RequestConfig const &src) {
	if (this != &src) {
		this->_contentLocation = src._contentLocation;
		this->_path = src._path;
		this->_error_page = src._error_page;
		this->_client_body_buffer_size = src._client_body_buffer_size;
		this->_cgi_param = src._cgi_param;
		this->_cgi_pass = src._cgi_pass;
		this->_allowed_methods = src._allowed_methods;
		this->_autoindex = src._autoindex;
		this->_index = src._index;
		this->_hostPort.host = src._hostPort.host;
		this->_hostPort.port = src._hostPort.port;
	}
	return *this;
}

RequestConfig::RequestConfig(RequestConfig const &src) {
	this->operator=(src);
}

RequestConfig::~RequestConfig() {
}

const std::string &RequestConfig::getContentLocation() const {
	return this->_contentLocation;
}

const std::string &RequestConfig::getPath() const {
	return this->_path;
}

const std::map<int, std::string> &RequestConfig::getErrorPage() const {
	return this->_error_page;
}

const unsigned long &RequestConfig::getClientBodyBufferSize() const {
	return this->_client_body_buffer_size;
}

const std::map<std::string, std::string> &RequestConfig::getCgiParam() const {
	return this->_cgi_param;
}

const std::string &RequestConfig::getCgiPass() const {
	return this->_cgi_pass;
}

const std::string &RequestConfig::getLang() const {
	return this->_lang;
}

const std::set<std::string> &RequestConfig::getAllowedMethods() const {
	return this->_allowed_methods;
}

const t_listen &RequestConfig::getHostPort() const {
	return this->_hostPort;
}

bool RequestConfig::getAutoIndex() const {
	return this->_autoindex;
}

void RequestConfig::setPath(int code) {
	this->_path = "./www/error_pages/"
			+ to_string(code) + ".html";
}

void RequestConfig::setPath(const std::string &path) {
	this->_path = path;
}

void RequestConfig::setContentLocation(const std::string &path) {
	this->_contentLocation = path;
}

void RequestConfig::setHostPort(const t_listen hostPort) {
	this->_hostPort.port = hostPort.port;
	this->_hostPort.host = hostPort.host;
}

std::string RequestConfig::addIndex() {
	std::string path;

	vectorOfStrings::iterator it = this->_index.begin();
	while (it != this->_index.end()) {
		path = this->_path;
		if (path[path.size() - 1] != '/' && pathIsFile(path) == 0)
			path.append("/");
		path += *it;
		if (pathIsFile(path)) {
			this->_path = path;
			if (this->_contentLocation[this->_contentLocation.size() - 1] != '/')
				this->_contentLocation += "/";
			this->_contentLocation += *it;
			return this->_path;
		}
		it++;
	}
	return "";
}

std::ostream &operator<<(std::ostream &out, RequestConfig &request) {
	out << "path: " << request._path << std::endl;
	out << "content_location: " << request._contentLocation << std::endl;
	out << "error_page:" << std::endl;
	std::map<int, std::string>::iterator i;
	for (i = request._error_page.begin(); i != request._error_page.end(); i++) {
		out << "\t" << i->first << " " << i->second << std::endl;
	}
	out << "client_body_buffer_size: " << request._client_body_buffer_size << std::endl;
	out << "cgi_param:" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = request._cgi_param.begin(); it != request._cgi_param.end(); it++)
		out << "\t" << it->first << "=" << it->second << std::endl;
	out << "cgi_pass: " << request._cgi_pass << std::endl << "allowed_methods:" << std::endl;
	std::set<std::string>::iterator ite;
	for (ite = request._allowed_methods.begin(); ite != request._allowed_methods.end(); ite++)
		out << "\t" << *ite << std::endl;
	out << "index:" << std::endl;
	vectorOfStrings::iterator iter;
	for (iter = request._index.begin(); iter != request._index.end(); iter++)
		out << "\t" << *iter << std::endl;
	if (request._autoindex) {
		out << "autoindex: " << "on" << std::endl;
	} else
		out << "autoindex: " << "off" << std::endl;
	return out;
}
