#ifndef REQUEST_CONFIG_HPP
# define REQUEST_CONFIG_HPP

#include "Request.hpp"
#include "Config.hpp"
#include "../webserv.hpp"

class RequestConfig {
private:
	std::string _path;

	std::string _cgi_pass;

	std::string _contentLocation;

	unsigned long _client_body_buffer_size;

	std::map<int, std::string> _error_page;

	std::map<std::string, std::string> _cgi_param;

	std::set<std::string> _allowed_methods;

	t_listen _hostPort;

	std::string _lang;

	std::vector<std::string> _index;

	bool _autoindex;

public:
	RequestConfig(void);

	RequestConfig(ConfigServer &config, Request &request,
				  const std::string &path,
				  const std::string &method,
				  std::string &locationName);

	RequestConfig(RequestConfig const &src);

	virtual ~RequestConfig(void);

	RequestConfig &operator=(RequestConfig const &src);

	friend std::ostream &operator<<(std::ostream &out,
									RequestConfig &request);

	const std::string &getContentLocation() const;

	const std::string &getPath() const;

	void setPath(int code);

	void setPath(const std::string &);

	const std::map<int, std::string> &getErrorPage() const;

	const unsigned long &getClientBodyBufferSize() const;

	const std::map<std::string, std::string> &getCgiParam() const;

	const std::string &getCgiPass() const;

	const std::string &getLang() const;

	const std::set<std::string> &getAllowedMethods() const;

	const t_listen &getHostPort() const;

	bool getAutoIndex() const;

	void setContentLocation(const std::string &);

	void setHostPort(const t_listen hostPort);

	std::string addIndex();


};

#endif
