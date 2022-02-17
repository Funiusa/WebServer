#include "ConfigServer.hpp"

ConfigServer::ConfigServer(void) :
		_root(""), _autoindex(false), _client_body_buffer_size(0), _aliasSet(false) {
}

ConfigServer::ConfigServer(ConfigServer const &src) {
	if (this != &src) {
		this->_listen = src._listen;
		this->_root = src._root;
		this->_server_name = src._server_name;
		this->_error_page = src._error_page;
		this->_client_body_buffer_size = src._client_body_buffer_size;
		this->_cgi_param = src._cgi_param;
		this->_cgi_pass = src._cgi_pass;
		this->_location = src._location;
		this->_allowed_methods = src._allowed_methods;
		this->_autoindex = src._autoindex;
		this->_index = src._index;
		this->_alias = src._alias;
		this->_aliasSet = src._aliasSet;
	}
}

ConfigServer &ConfigServer::operator=(ConfigServer const &src) {
	if (this != &src) {
		this->_listen = src._listen;
		this->_root = src._root;
		this->_server_name = src._server_name;
		this->_error_page = src._error_page;
		this->_client_body_buffer_size = src._client_body_buffer_size;
		this->_cgi_param = src._cgi_param;
		this->_cgi_pass = src._cgi_pass;
		this->_location = src._location;
		this->_allowed_methods = src._allowed_methods;
		this->_autoindex = src._autoindex;
		this->_index = src._index;
		this->_alias = src._alias;
		this->_aliasSet = src._aliasSet;
	}
	return *this;
}

ConfigServer::~ConfigServer() {
}


ConfigServer    ConfigServer::_defaultServer = ConfigServer();
parseMap ConfigServer::serverParsingMap = ConfigServer::_initServerMap();
parseMap ConfigServer::locationParsingMap = ConfigServer::_initLocationMap();

parseMap ConfigServer::_initServerMap() {
	parseMap myMap;
	myMap["listen"] = &ConfigServer::addListen;
	myMap["root"] = &ConfigServer::addRoot;
	myMap["server_name"] = &ConfigServer::addServerName;
	myMap["error_page"] = &ConfigServer::addErrorPage;
	myMap["client_body_buffer_size"] = &ConfigServer::addClientBodyBufferSize;
	myMap["cgi_param"] = &ConfigServer::addCgiParam;
	myMap["cgi_pass"] = &ConfigServer::addCgiPass;
	myMap["allow_methods"] = &ConfigServer::addAllowedMethods;
	myMap["index"] = &ConfigServer::addIndex;
	myMap["autoindex"] = &ConfigServer::addAutoIndex;
	return myMap;
}

parseMap ConfigServer::_initLocationMap() {
	parseMap myMap;
	myMap["root"] = &ConfigServer::addRoot;
	myMap["error_page"] = &ConfigServer::addErrorPage;
	myMap["client_body_buffer_size"] = &ConfigServer::addClientBodyBufferSize;
	myMap["cgi_param"] = &ConfigServer::addCgiParam;
	myMap["cgi_pass"] = &ConfigServer::addCgiPass;
	myMap["allow_methods"] = &ConfigServer::addAllowedMethods;
	myMap["index"] = &ConfigServer::addIndex;
	myMap["autoindex"] = &ConfigServer::addAutoIndex;
	myMap["alias"] = &ConfigServer::addAlias;
	return myMap;
}

ConfigServer ConfigServer::_initDefaultServer(const std::string &filename) {
	ConfigServer server;
	vectorOfStrings file;

	file = ConfigReader::readFile(filename);
	if (file.empty()) {
		std::cerr << RED << "Could not open default file at location [" << filename << "]" << RESET << std::endl;
		throw ConfigReader::FileNotFoundException();
	}
	file.insert(file.begin(), "{");
	file.insert(file.begin(), "server");
	file.insert(file.end(), "}");
	unsigned int index = 2;
	if (!server.parseServer(index, file)) {
		std::cerr << RED << "Invalid default config file." << RESET << std::endl;
		throw ConfigServer::ExceptionInvalidArguments();
	}
	ConfigServer::_defaultServer = server;
	return server;
}

int ConfigServer::parseServer(unsigned int &index, vectorOfStrings &file) {
	vectorOfStrings args;
	parseMap::iterator iter;
	std::string directive;

	for (; index < file.size() && file[index] != "}"; index++) {
		if ((iter = ConfigServer::serverParsingMap.find(file[index])) == ConfigServer::serverParsingMap.end()) {
			if (file[index] == "location") {
				ConfigServer location;
				std::string locationName;

				if (!directive.empty()) {
					(this->*ConfigServer::serverParsingMap[directive])(args);
					args.clear();
					directive = "";
				}
				index++;
				if (file[index] == "{" || file[index] == "}")
					return 0;
				locationName = file[index];
				index++;
				if (!location.parseLocation(index, file))
					return 0;
				this->_location[locationName] = location;
				if (file[index] == "}")
					continue;
			} else if (!directive.compare(""))
				return file[index] == "}" ? 1 : 0;
			else
				args.push_back(file[index]);
		} else {
			if (!directive.empty()) {
				(this->*ConfigServer::serverParsingMap[directive])(args);
				args.clear();
			}
			directive = iter->first;
		}
	}
	if (!directive.empty())
		(this->*ConfigServer::serverParsingMap[directive])(args);
	if (!file[index].compare("}")) {
		ConfigServer::_defaultServer.passMembers(*this);
		std::map<std::string, ConfigServer>::iterator it;
		for (it = this->_location.begin(); it != this->_location.end(); it++)
			this->passMembers(it->second);
		return 1;
	}
	return 0;
}

void ConfigServer::passMembers(ConfigServer &server) const {
	if (this != &server) {
		if (server._listen.empty())
			server._listen.insert(server._listen.begin(),
								  this->_listen.begin(), this->_listen.end());
		if (server._root.empty())
			server._root = this->_root;
		server._server_name.insert(server._server_name.end(),
								   this->_server_name.begin(), this->_server_name.end());
		std::map<int, std::string>::const_iterator i;
		for (i = this->_error_page.begin(); i != this->_error_page.end(); i++) {
			if (server._error_page.find(i->first) == server._error_page.end())
				server._error_page[i->first] = i->second;
		}
		if (server._client_body_buffer_size == 0)
			server._client_body_buffer_size = this->_client_body_buffer_size;
		std::map<std::string, std::string>::const_iterator it;
		for (it = this->_cgi_param.begin(); it != this->_cgi_param.end(); it++) {
			if (server._cgi_param.find(it->first) == server._cgi_param.end())
				server._cgi_param[it->first] = it->second;
		}
		if (server._cgi_pass.empty())
			server._cgi_pass = this->_cgi_pass;
		if (server._allowed_methods.empty())
			server._allowed_methods = this->_allowed_methods;
		server._index.insert(server._index.begin(), this->_index.begin(),
							 this->_index.end());
	}
	std::map<std::string, ConfigServer>::iterator iter;
	for (iter = server._location.begin(); iter != server._location.end(); iter++)
		server.passMembers(iter->second);
}

int ConfigServer::parseLocation(unsigned int &index, vectorOfStrings &file) {
	vectorOfStrings args;
	std::string directive;
	parseMap::iterator parseMapIter;

	if (file[index++] != "{")
		return 0;
	for (; index < file.size() && file[index] != "}"; index++) {
		parseMapIter = ConfigServer::locationParsingMap.find(file[index]);
		if (parseMapIter == ConfigServer::locationParsingMap.end()) {
			if (file[index] == "location") {
				ConfigServer location;
				std::string locationName;

				if (!directive.empty()) {
					(this->*ConfigServer::locationParsingMap[directive])(args);
					args.clear();
					directive = "";
				}
				index++;
				if (file[index] == "{" || file[index] == "}")
					return 0;
				locationName = file[index];
				index++;
				if (!location.parseLocation(index, file))
					return 0;
				this->_location[locationName] = location;
				if (file[index] == "}")
					continue;
			} else if (directive.empty())
				return file[index] == "}" ? 1 : 0;
			else
				args.push_back(file[index]);
		} else {
			if (!directive.empty()) {
				(this->*ConfigServer::locationParsingMap[directive])(args);
				args.clear();
			}
			directive = parseMapIter->first;
		}
	}
	if (!directive.empty())
		(this->*ConfigServer::locationParsingMap[directive])(args);
	if (!file[index].compare("}"))
		return 1;
	return 0;
}

bool isDigits(const std::string &str) {
	return str.find_first_not_of("0123456789") == std::string::npos;
}

void ConfigServer::addListen(std::vector<std::string> args) {
	t_listen listen;
	size_t sep;

	if (args.size() != 1)
		throw ConfigServer::ExceptionInvalidArguments();
	if ((sep = args[0].find(":")) == std::string::npos) {
		if (isDigits(args[0])) {
			listen.host = 0;
			listen.port = (int) strtol(args[0].c_str(), NULL, 10);
			std::vector<t_listen>::const_iterator it;
			for (it = _listen.begin(); it != _listen.end(); it++) {
				if (it->port == listen.port)
					throw ConfigServer::ExceptionInvalidArguments();
			}
			this->_listen.push_back(listen);
			return;
		}
		throw ConfigServer::ExceptionInvalidArguments();
	} else {
		listen.host = strToIp(args[0].substr(0, sep));
		sep++;
		std::string portStr = args[0].substr(sep);
		if (isDigits(portStr)) {
			listen.port = (int) strtol(portStr.c_str(), NULL, 10);
			this->_listen.push_back(listen);
			return;
		}
		throw ConfigServer::ExceptionInvalidArguments();
	}
}

void ConfigServer::addRoot(std::vector<std::string> args) {
	if (args.size() != 1 || !this->_root.empty())
		throw ConfigServer::ExceptionInvalidArguments();
	this->_root = args[0];
}

void ConfigServer::addServerName(std::vector<std::string> args) {
	if (args.empty())
		throw ConfigServer::ExceptionInvalidArguments();
	for (unsigned int i = 0; i < args.size(); i++)
		this->_server_name.push_back(args[i]);
}

void ConfigServer::addErrorPage(std::vector<std::string> args) {
	std::string uri;
	std::vector<int> codes;
	size_t len = args.size();

	for (size_t i = 0; i < len; i++) {
		if (isDigits(args[i]))
			codes.push_back((int) strtol(args[i].c_str(), NULL, 10));
		else if (codes.empty())
			throw ConfigServer::ExceptionInvalidArguments();
		else if (i == (len - 1))
			uri = args[i];
		else
			throw ConfigServer::ExceptionInvalidArguments();
	}
	if (uri.empty())
		throw ConfigServer::ExceptionInvalidArguments();
	std::vector<int>::iterator i;
	for (i = codes.begin(); i != codes.end(); i++)
		this->_error_page[*i] = uri;
}

void ConfigServer::addClientBodyBufferSize(vectorOfStrings args) {
	if (args.size() != 1 || !isDigits(args[0]))
		throw ConfigServer::ExceptionInvalidArguments();
	this->_client_body_buffer_size = (int) strtol(args[0].c_str(), NULL, 10);
}

void ConfigServer::addCgiParam(vectorOfStrings args) {
	if (args.size() != 2)
		throw ConfigServer::ExceptionInvalidArguments();
	this->_cgi_param[args[0]] = args[1];
}

void ConfigServer::addCgiPass(vectorOfStrings args) {
	if (args.size() != 1)
		throw ConfigServer::ExceptionInvalidArguments();
	this->_cgi_pass = args[0];
}

void ConfigServer::addAllowedMethods(vectorOfStrings args) {
	if (args.empty())
		throw ConfigServer::ExceptionInvalidArguments();
	this->_allowed_methods.clear();
	vectorOfStrings::iterator i;
	for (i = args.begin(); i != args.end(); i++)
		this->_allowed_methods.insert(*i);
}

void ConfigServer::addIndex(vectorOfStrings args) {
	if (args.empty())
		throw ConfigServer::ExceptionInvalidArguments();
	this->_index.insert(this->_index.end(), args.begin(), args.end());
}

void ConfigServer::addAutoIndex(vectorOfStrings args) {
	if (args.size() != 1)
		throw ConfigServer::ExceptionInvalidArguments();
	if (args[0] == "on")
		this->_autoindex = true;
	else if (args[0] == "off")
		this->_autoindex = false;
	else { throw ConfigServer::ExceptionInvalidArguments(); }
}

void ConfigServer::addAlias(vectorOfStrings args) {
	if (args.size() > 1)
		throw ConfigServer::ExceptionInvalidArguments();
	if (!args.empty())
		this->_alias = args[0];
	this->_aliasSet = true;
}

std::ostream &operator<<(std::ostream &out, const ConfigServer &server) {
	out << "Listen:" << std::endl;
	for (size_t i = 0; i < server._listen.size(); i++) {
		out << "\thost: " << server._listen[i].host << " port: " << server._listen[i].port << std::endl;
	}
	out << "root: " << server._root << std::endl;
	out << "server_name: ";
	for (size_t i = 0; i < server._server_name.size(); i++) {
		out << server._server_name[i];
		if (i != server._server_name.size() - 1)
			out << " ";
	}
	out << std::endl << "error_page:" << std::endl;
	std::map<int, std::string>::const_iterator i;
	for (i = server._error_page.begin();
		 i != server._error_page.end(); i++) {
		out << "\t" << i->first << " " << i->second << std::endl;
	}
	out << "client_body_buffer_size: " << server._client_body_buffer_size << std::endl;
	out << "cgi_param:" << std::endl;
	std::map<std::string, std::string>::const_iterator it;
	for (it = server._cgi_param.begin(); it != server._cgi_param.end(); it++)
		out << "\t" << it->first << " = " << it->second << std::endl;
	out << "cgi_pass:	" << server._cgi_pass << std::endl;
	out << "allowed methods: ";
	std::set<std::string>::iterator iter;
	for (iter = server._allowed_methods.begin(); iter != server._allowed_methods.end(); iter++)
		out << " " << *iter << std::endl;
	out << "autoindex " << (server._autoindex ? "on" : "off") << std::endl;
	out << "index: ";
	vectorOfStrings::const_iterator itr;
	for (itr = server._index.begin(); itr != server._index.end(); itr++)
		out << *itr << " " << std::endl;
	out << "alias: " << server._alias << std::endl;
	for (std::map<std::string, ConfigServer>::const_iterator
				 i = server._location.begin(); i != server._location.end(); i++) {
		out << std::endl << "LOCATION " << i->first << std::endl;
		out << i->second << std::endl;
	}
	return out;
}

ConfigServer ConfigServer::getLocationForRequest(std::string const &path,
												 std::string &retLocationPath) {
	std::string tryLocation;
	std::string::size_type tryLen = path.length();
	std::map<std::string, ConfigServer>::iterator iter;

	if (!tryLen)
		return *this;
	if (!this->_location.empty()) {
		do {
			tryLocation = path.substr(0, tryLen);
			iter = this->_location.find(tryLocation);
			if (iter != this->_location.end() && iter->first[0] != '*') {
				retLocationPath = tryLocation;
				return iter->second.getLocationForRequest(path, retLocationPath);
			}
			tryLen--;
		} while (tryLen);
		iter = this->_location.begin();
		do {
			if (iter->first[0] == '*') {
				std::string suffix = iter->first.substr(1);
				if (path.length() > suffix.length() &&
					!path.compare(path.length() - suffix.length(), suffix.length(), suffix)) {
					return iter->second.getLocationForRequest(path, retLocationPath);
				}
			}
			iter++;
		} while (iter != this->_location.end());
	}
	return (*this);
}

std::vector<t_listen> ConfigServer::getListen() const {
	return this->_listen;
}

std::string ConfigServer::getRoot() const {
	return this->_root;
}

vectorOfStrings ConfigServer::getServerName() const {
	return this->_server_name;
}

std::map<int, std::string> ConfigServer::getErrorPage() const {
	return this->_error_page;
}

int ConfigServer::getClientBodyBufferSize() const {
	return this->_client_body_buffer_size;
}

std::map<std::string, std::string> ConfigServer::getCgiParam() const {
	return this->_cgi_param;
}

std::string ConfigServer::getCgiPass() const {
	return this->_cgi_pass;
}

std::map<std::string, ConfigServer> ConfigServer::getLocation() const {
	return this->_location;
}

std::set<std::string> ConfigServer::getAllowedMethods() const {
	return this->_allowed_methods;
}

vectorOfStrings ConfigServer::getIndex() const {
	return this->_index;
}

bool ConfigServer::getAutoIndex() const {
	return this->_autoindex;
}

std::string ConfigServer::getAlias() const {
	return this->_alias;
}

bool ConfigServer::getAliasSet() const {
	return this->_aliasSet;
}

ConfigServer &ConfigServer::getDefaultServer() {
	return ConfigServer::_defaultServer;
}

const char *ConfigServer::ExceptionInvalidArguments::what()
const throw() {
	return "Invalid arguments in configuration file";
}
