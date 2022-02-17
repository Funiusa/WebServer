#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "../webserv.hpp"
# include "ConfigReader.hpp"
# include "ConfigServer.hpp"
# include "RequestConfig.hpp"

#define DEFAULT_CONFIG "/home/denis/Desktop/Serv/www/conf/default.conf"

class Config {
private:
	bool getServerForRequest(ConfigServer &ret, t_listen const address,
							 std::string const &hostName) const;

	std::vector<ConfigServer> _servers;
public:
	Config(const std::string &defaultServerPath = DEFAULT_CONFIG);

	Config(Config const &);

	virtual ~Config(void);

	Config &operator=(Config const &);

	friend std::ostream &operator<<(std::ostream &out, const Config &config);

	int parse(const std::string &);

	std::vector<ConfigServer> getServers() const;

	RequestConfig getConfigForRequest(t_listen const address, std::string const &uri,
									  std::string hostName, const std::string &method,
									  Request &request) const;

	std::vector<t_listen> getAllListens() const;
};

std::ostream &operator<<(std::ostream &out, const Config &config);

#endif
