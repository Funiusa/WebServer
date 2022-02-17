#pragma once

#include "../webserv.hpp"

#ifdef DEBUG
# define PRINT 1
#else
# define PRINT 0
#endif

class Request {
private:
	int _ret;
	int _port;
	std::string _bodyRequest;
	std::string _path;
	std::string _query;
	std::string _method;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::map<std::string, std::string> _env_for_cgi;
	std::list<std::pair<std::string, float> > _lang;
	const std::string &_raw;

	int readFirstLine(const std::string &line);

	int readPath(const std::string &line, size_t i);

	int readVersion(const std::string &line, size_t i);

	int checkMethod();

	void findQuery();

	std::string formatHeaderForCGI(std::string &key);

	std::string nextLineInString(const std::string &str, size_t &i);

	static std::vector<std::string> methods;

	Request();

	Request(const Request &);

public:
	Request(const std::string &str);

	~Request();

	Request &operator=(const Request &);

	const std::map<std::string, std::string> &getHeaders() const;

	const std::map<std::string, std::string> &getEnv() const;

	const std::string &getMethod() const;

	const std::string &getVersion() const;

	int getRet() const;

	const std::string &getBody() const;

	int getPort() const;

	const std::string &getPath() const;

	const std::string &getQuery() const;

	void setBody(const std::string &line);

	void setMethod(const std::string &method);

	int parseRequestStr(const std::string &str);

	void resetHeaders();

	static std::vector<std::string> initMethods();

};

std::ostream &operator<<(std::ostream &os, const Request &re);
