#include "ResponseHeader.hpp"

std::string
ResponseHeader::getHeader(size_t size, const std::string &path,
						  int code, const std::string &type,
						  const std::string &contentLocation,
						  const std::string &lang) {
	std::string header;
	resetValues();
	setValues(size, path, code, type, contentLocation, lang);
	header = "HTTP/1.1 " + to_string(code)
			 + " " + getStatusMessage(code) + "\r\n";
	header += writeHeader();
	return (header);
}

std::string ResponseHeader::writeHeader() {
	std::string header;

	if (!_allow.empty())
		header += "Allow: " + _allow + "\r\n";
	if (!_contentLanguage.empty())
		header += "Content-Language: " + _contentLanguage + "\r\n";
	if (!_contentLength.empty())
		header += "Content-Length: " + _contentLength + "\r\n";
	if (!_contentLocation.empty())
		header += "Content-Location: " + _contentLocation + "\r\n";
	if (!_contentType.empty())
		header += "Content-Type: " + _contentType + "\r\n";
	if (!_date.empty())
		header += "Date: " + _date + "\r\n";
	if (!_lastModified.empty())
		header += "Last-Modified: " + _lastModified + "\r\n";
	if (!_location.empty())
		header += "Location: " + _location + "\r\n";
	if (!_retryAfter.empty())
		header += "Retry-After: " + _retryAfter + "\r\n";
	if (!_server.empty())
		header += "Server: " + _server + "\r\n";
	if (!_transferEncoding.empty())
		header += "Transfer-Encoding: " + _transferEncoding + "\r\n";
	if (!_wwwAuthenticate.empty())
		header += "WWW-Authenticate: " + _wwwAuthenticate + "\r\n";
	return (header);
}

std::string ResponseHeader::getStatusMessage(int code) {
	if (_errors.find(code) != _errors.end())
		return _errors[code];
	return ("Unknown Code");
}

void ResponseHeader::initErrorMap() {
	_errors[200] = "OK";
	_errors[201] = "Created";
	_errors[204] = "No Content";
	_errors[100] = "Continue";
	_errors[403] = "Forbidden";
	_errors[404] = "Not Found";
	_errors[400] = "Bad Request";
	_errors[408] = "Timeout";
	_errors[413] = "Payload Too Large";
	_errors[405] = "Method Not Allowed";
	_errors[500] = "Internal Server Error";
}

void
ResponseHeader::setValues(size_t size, const std::string &path,
						  int code, const std::string &type,
						  const std::string &contentLocation,
						  const std::string &lang) {
	setDate();
	setAllow();
	setServer();
	setTransferEncoding();
	setLastModified(path);
	setContentLength(size);
	setWwwAuthenticate(code);
	setContentLanguage(lang);
	setContentType(type, path);
	setRetryAfter(code, 3);
	setLocation(code, path);
	setContentLocation(contentLocation);
}

void ResponseHeader::resetValues() {
	_server = "";
	_date = "";
	_allow = "";
	_contentLanguage = "";
	_contentLength = "";
	_contentLocation = "";
	_contentType = "";
	_lastModified = "";
	_location = "";
	_retryAfter = "";
	_transferEncoding = "";
	_wwwAuthenticate = "";
	initErrorMap();
}

void ResponseHeader::setAllow(const std::string &allow) {
	_allow = allow;
}

void ResponseHeader::setContentLanguage(const std::string &lang) {
	_contentLanguage = lang;
}

void ResponseHeader::setContentLength(size_t size) {
	_contentLength = to_string(size);
}

void ResponseHeader::setContentLocation(const std::string &path) {
	_contentLocation = path;
}

void ResponseHeader::setContentType(std::string type,
									const std::string &path) {
	if (!type.empty()) {
		_contentType = type;
		return;
	}
	type = path.substr(path.rfind('.') + 1, path.size() - path.rfind('.'));
	if (type == "html")
		_contentType = "text/html";
	else if (type == "css")
		_contentType = "text/css";
	else if (type == "js")
		_contentType = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		_contentType = "image/jpeg";
	else if (type == "png")
		_contentType = "image/png";
	else if (type == "bmp")
		_contentType = "image/bmp";
	else
		_contentType = "text/plain";
}

void ResponseHeader::setDate() {
	struct timeval tv = {};
	char buffer[100];
	struct tm *tm;

	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
	_date = std::string(buffer);
}

void ResponseHeader::setLastModified(const std::string &path) {
	struct stat stats = {};
	char buffer[100];
	struct tm *tm;

	if (stat(path.c_str(), &stats) == 0) {
		tm = gmtime(&stats.st_mtime);
		strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
		_lastModified = std::string(buffer);
	}
}

void ResponseHeader::setLocation(int code,
								 const std::string &redirect) {
	if (code == 201 || code / 100 == 3)
		_location = redirect;
}

void ResponseHeader::setRetryAfter(int code, int sec) {
	if (code == 503 || code == 429 || code == 301)
		_retryAfter = to_string(sec);
}

void ResponseHeader::setServer() {
	_server = "NONGINX/1.0";
}

void ResponseHeader::setTransferEncoding() {
	_transferEncoding = "identity";
}

void ResponseHeader::setWwwAuthenticate(int code) {
	if (code == 401) {
		_wwwAuthenticate = "Basic realm=\"Access requires authentication\" charset=\"UTF-8\"";
	}
}

ResponseHeader &ResponseHeader::operator=(const ResponseHeader &src) {
	(void) src;
	return (*this);
}

ResponseHeader::ResponseHeader() {
	resetValues();
}

ResponseHeader::ResponseHeader(const ResponseHeader &src) {
	this->operator=(src);
}

ResponseHeader::~ResponseHeader() {
}
