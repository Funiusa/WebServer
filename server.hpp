
#ifndef WEBSERVER_SERVER_HPP
#define WEBSERVER_SERVER_HPP

#include <fstream>
//#include <bits/socket.h>

#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <sys/socket.h>
#include <sys/poll.h>
#include <fcntl.h>

#define TRUE             1
#define FALSE            0

class Server {
protected:
	int	_rc;
	int	_listenSD;
	int	_timeout;
	int _port;
	struct pollfd _fds[200];
	int	_nfds;
	int _endServer;
	int _closeConnection;
	int _currentSize;
	int _newSd;
	int _compressArray;
public:
	Server();
	Server(int port);
	~Server();

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
