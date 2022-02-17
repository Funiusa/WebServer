#include "Config.hpp"

Config::Config(const std::string &defaultServerPath) {
	try {
		ConfigServer::_initDefaultServer(defaultServerPath);
	}
	catch (ConfigReader::FileNotFoundException &e) {
		std::cerr << RED << "Could not open default config file." << RESET << std::endl;
	}
}

Config::Config(Config const &src) {
	if (this != &src)
		this->_servers = src._servers;
}

int Config::parse(const std::string &filename) {
	vectorOfStrings file;
	unsigned int fileSize;

	file = ConfigReader::readFile(filename);
	fileSize = file.size();
	for (unsigned int i = 0; i < fileSize; i++) {
		if (file[i] == "server") {
			ConfigServer server;
			++i;
			if (file[i] != "{") {
				std::cerr << RED << "Error: expected '{' after server directive." << RESET << std::endl;
				return 1;
			}
			++i;
			if (!server.parseServer(i, file)) {
				std::cerr << RED << "Error: error in config file [" << filename << "]" << RESET << std::endl;
				return 1;
			}
			this->_servers.push_back(server);
		} else {
			std::cerr << RED << "Error: unknown directive [" << file[i] << "]" << RESET << std::endl;
			return 1;
		}
	}
	return 0;
}

RequestConfig Config::getConfigForRequest(t_listen const address, std::string const &uri, std::string hostName,
										  const std::string &method, Request &request) const {
	ConfigServer server;
	std::string locationPath;

	hostName = hostName.substr(0, hostName.find_last_of(':'));
	this->getServerForRequest(server, address, hostName);
	server = server.getLocationForRequest(uri, locationPath);
	if (*(--locationPath.end()) == '/')
		locationPath.resize(locationPath.size() - 1);
	RequestConfig config(server, request, uri, method, locationPath);
	config.setHostPort(address);
	return config;
}

bool Config::getServerForRequest(ConfigServer &ret, t_listen const address, std::string const &hostName) const {
	std::vector<ConfigServer> possibleServers;
	std::vector<ConfigServer>::const_iterator serversIter;
	serversIter = this->_servers.begin();
	for (; serversIter != this->_servers.end(); serversIter++) {
		std::vector<t_listen> listens = serversIter->getListen();
		std::vector<t_listen>::iterator listenIter;
		listenIter = listens.begin();
		for (; listenIter != listens.end(); listenIter++) {
			if (address.host == (*listenIter).host && address.port == (*listenIter).port) {
				possibleServers.push_back((*serversIter));
			}
		}
	}
	if (possibleServers.empty())
		return false;
	serversIter = possibleServers.begin();
	while (serversIter != possibleServers.end()) {
		vectorOfStrings serverNames = serversIter->getServerName();
		vectorOfStrings::iterator servNameIter = serverNames.begin();
		while (servNameIter != serverNames.end()) {
			if (*servNameIter == hostName) {
				ret = *serversIter;
				return true;
			}
			servNameIter++;
		}
		serversIter++;
	}
	ret = possibleServers[0];
	return true;
}

std::vector<t_listen> Config::getAllListens() const {
	std::vector<ConfigServer>::const_iterator server;
	std::vector<t_listen> ret;
	for (server = this->_servers.begin(); server != this->_servers.end(); server++) {
		std::vector<t_listen> listenVec = server->getListen();
		std::vector<t_listen>::iterator listen;
		for (listen = listenVec.begin(); listen != listenVec.end(); listen++) {
			std::vector<t_listen>::iterator elem;
			for (elem = ret.begin(); elem != ret.end(); elem++)
				if (listen->host == elem->host && listen->port == elem->port)
					break;
			if (elem == ret.end()) { ret.push_back(*listen); }
		}
	}
	return ret;
}

std::ostream &operator<<(std::ostream &out, const Config &config) {
	out << "DEFAULT_CONFIG\n" << ConfigServer::getDefaultServer() << '\n';
	for (size_t index = 0; index < config._servers.size(); index++) {
		out << "SERVER " << index << std::endl;
		out << config._servers[index] << std::endl;
	}
	return out;
}

Config &Config::operator=(Config const &src) {
	if (this != &src)
		this->_servers = src._servers;
	return (*this);
}

Config::~Config() {
}

std::vector<ConfigServer> Config::getServers() const {
	return this->_servers;
}

