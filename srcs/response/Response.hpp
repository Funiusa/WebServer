#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../webserv.hpp"
# include "ResponseHeader.hpp"
# include "Request.hpp"
# include "RequestConfig.hpp"
# include "CgiHandler.hpp"
# include "AutoIndexGenerator.hpp"


class Response {
private:
	int _code;

	std::string _response;

	std::string _path;

	std::string _type;

	bool _isAutoIndex;

	t_listen _hostPort;

	std::map<int, std::string> _errorMap;

	static std::map<std::string,
			void (Response::*)(Request &, RequestConfig &)> _method;

	static std::map<std::string,
			void (Response::*)(Request &, RequestConfig &)> initMethods();

public:
	Response();

	Response(const Response &src);

	~Response();

	Response &operator=(const Response &src);

	std::string getResponse();

	void answerToClient(Request &request, RequestConfig &requestConf);

	void getMethod(Request &request, RequestConfig &requestConf);

	void headMethod(Request &request, RequestConfig &requestConf);

	void postMethod(Request &request, RequestConfig &requestConf);

	void putMethod(Request &request, RequestConfig &requestConf);

	void deleteMethod(Request &request, RequestConfig &requestConf);

	int readContent();

	int writeContent(const std::string &content);

	std::string readHtml(const std::string &path);


};

#endif
