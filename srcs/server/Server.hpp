#ifndef SERVER_HPP
#define SERVER_HPP

#include "../webserv.hpp"

#include "ConfigReader.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "RequestConfig.hpp"
#include "CgiHandler.hpp"

class Server {
private:
	std::map<long, std::string> _requests;
	t_listen _listen;
	int _fd;
	int _optVal;
	struct sockaddr_in _addr;

	Server();

public:
	Server(const t_listen &listen);

	Server(const Server &src);

	virtual ~Server();

	Server &operator=(const Server &src);

	int createListeningSocket();

	int socketDescriptor();

	int setSockOpt();

	int nonBlockSocket();

	int socketBind();

	int socketListener();

	long acceptIncomingConnection();

	void process(long socket, Config &conf);

	void processChunk(long socket);

	int receiveIncomingData(long socket);

	int echoDataToClient(long socket);

	long getFD() const;

	void closeSockFD(int socket);

	void clean();

};

#endif
