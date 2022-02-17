#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include "../webserv.hpp"

# include "Config.hpp"

# include "Request.hpp"
# include "RequestConfig.hpp"


class CgiHandler {
private:
	std::map<std::string, std::string> _env;

	std::string _body;

	CgiHandler();

	void fillEnvReq(Request &request,
					RequestConfig &config);

	char **envStringMap() const;

public:
	CgiHandler(CgiHandler const &src);

	CgiHandler(Request &request,
			   RequestConfig &config);

	virtual ~CgiHandler();

	CgiHandler &operator=(CgiHandler const &src);

	std::string executeCgi(const std::string &scriptName);
};

#endif
