#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

#include "../webserv.hpp"

class ResponseHeader {
private:
	std::string _allow;

	std::string _contentLanguage;

	std::string _contentLength;

	std::string _contentLocation;

	std::string _contentType;

	std::string _date;

	std::string _lastModified;

	std::string _location;

	std::string _retryAfter;

	std::string _server;

	std::string _transferEncoding;

	std::string _wwwAuthenticate;

	std::map<int, std::string> _errors;

	void initErrorMap();

public:
	ResponseHeader();

	ResponseHeader(const ResponseHeader &src);

	~ResponseHeader();

	ResponseHeader &operator=(const ResponseHeader &src);

	void setAllow(const std::string &allow = "");

	void setContentLanguage(const std::string &lang = "");

	void setContentLength(size_t size);

	void setContentLocation(const std::string &path);

	void setContentType(std::string type, const std::string &path);

	void setDate();

	void setLastModified(const std::string &path);

	void setLocation(int code, const std::string &redirect);

	void setRetryAfter(int code, int sec);

	void setServer();

	void setTransferEncoding();

	void setWwwAuthenticate(int code);

	std::string getHeader(size_t size, const std::string &path,
						  int code, const std::string &type,
						  const std::string &contentLocation,
						  const std::string &lang);

	std::string writeHeader();

	void setValues(size_t size, const std::string &path,
				   int code, const std::string &type,
				   const std::string &contentLocation,
				   const std::string &lang);

	void resetValues();

	std::string getStatusMessage(int code);

};

#endif
