#ifndef WEBSERVER_SERVER_HPP
#define WEBSERVER_SERVER_HPP

#include <fstream>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sstream>
#include <fcntl.h>

#define MAX_CONNECTION 1000
#define nullptr NULL
#define TRUE             1
#define FALSE            0


class Server {
protected:
	int	_rc;
	int _aindex;
	int	_listenSD;
	int	_timeout;
	int _port;
	struct pollfd _fds[200];
	int	_nfds;
	int _endServer;
	int _currentSize;
	int _newSd;
	int _compressArray;
	int _closeConnection;
	std::string prevDir;
	std::string _location;
public:

	Server();
	Server(int port);
	virtual ~Server() { };

	void pollInit();
	void pollWait();
	void incomingConnectionLoop();
	void pollinDescriptorsLoop();
	int acceptIncomingConnections();
	void addNewIncomingConnection();
	void dataReceive();
	void closeConnection(int fd);
	void compressFdArray();
	void cleanUpAllOpenSockets();

	void setListenSd(int listenSd);
	void setAutoindex(int val);
	void setLocation(const std::string&);
	std::string getLocation() const;
	int getAutoindex();
	bool socketInit();
	bool clientProcessing(int aSocket);
	void start();

	class myErrorException: public std::exception {
	private:
		std::string _errorString;
	public:
			myErrorException(const std::string& errorString): _errorString(errorString) {};
			virtual ~myErrorException() throw() {};
			virtual const char* what() const throw();
	};
private:
	std::string m_start_data_time;
	size_t m_get_request_count;


};


#endif //WEBSERVER_SERVER_HPP
