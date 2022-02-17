#include "Cluster.hpp"
#include <cstring>

#define FALSE false
#define TRUE true

void Cluster::config(const std::string &conFile) {
	_config.parse(conFile);
}

/*************************************************************/
/* Initialize the master fd_set                              */
/*************************************************************/
int Cluster::createServer() {
	std::vector<t_listen> vect = _config.getAllListens();
	FD_ZERO(&_fd_set);
	_max_fd = 0;
	_fd_size = vect.size();

	std::vector<t_listen>::const_iterator lstn;
	for (lstn = vect.begin(); lstn != vect.end(); lstn++) {
		Server serv(*lstn);
		if (serv.createListeningSocket() != -1) {
			long fd = serv.getFD();
			FD_SET(fd, &_fd_set);
			_servers.insert(std::make_pair(fd, serv));
			if (fd > _max_fd)
				_max_fd = fd;
		}
	}
	if (_max_fd == 0) {
		std::cerr << RED << "\nCould not setup cluster" << RESET << std::endl;
		return (-1);
	} else
		return (0);
}


void Cluster::sendingData(fd_set writing_set, fd_set reading_set) {
	std::vector<int>::iterator it;
	for (it = _ready.begin(); it != _ready.end(); it++) {
		if (FD_ISSET(*it, &writing_set)) {
			long ret = _sockets[*it]->echoDataToClient(*it);
			if (ret == 0)
				_ready.erase(it);
			else if (ret == -1) {
				FD_CLR(*it, &_fd_set);
				FD_CLR(*it, &reading_set);
				_sockets.erase(*it);
				_ready.erase(it);
			}
			break;
		}
	}
}

void Cluster::receiveData(fd_set reading_set) {

	std::map<long, Server *>::iterator it;
	for (it = _sockets.begin(); it != _sockets.end(); it++) {
		int socket = (int) it->first;

		if (FD_ISSET(socket, &reading_set)) {
			std::cout << "\rReceived a connection" << std::flush;
			long ret = it->second->receiveIncomingData(socket);

			if (ret == 0) {
				it->second->process(socket, _config);
				_ready.push_back(socket);
			} else if (ret == -1) {
				FD_CLR(socket, &_fd_set);
				FD_CLR(socket, &reading_set);
				_sockets.erase(socket);
			}
			break;
		}
	}
}

void Cluster::acceptedIncomingConnection(fd_set reading_set) {
	std::map<long, Server>::iterator it;
	for (it = _servers.begin(); it != _servers.end(); it++) {
		long fd = it->first;

		if (FD_ISSET(fd, &reading_set)) {
			long new_sd = it->second.acceptIncomingConnection();
			/**********************************************/
			/* Add the new incoming connection to the     */
			/* master read set                            */
			/**********************************************/
			if (new_sd != -1) {
				FD_SET(new_sd, &_fd_set);
				_sockets.insert(std::make_pair(new_sd, &(it->second)));
				if (new_sd > _max_fd)
					_max_fd = new_sd;
			}
			break;
		}
	}
}

/*************************************************/
/* If the close_conn flag was turned on, we need */
/* to clean up this active connection.  This     */
/* clean up process includes removing the        */
/* descriptor from the master set and            */
/* determining the new maximum descriptor value  */
/* based on the bits that are still turned on in */
/* the master set.                               */
/*************************************************/
void Cluster::closeFailedConnection() {
	std::cerr << RED << "\nselect() failed" << RESET << std::endl;
	std::map<long, Server *>::iterator it;
	for (it = _sockets.begin(); it != _sockets.end(); it++)
		it->second->closeSockFD((int) it->first);
	_sockets.clear();
	_ready.clear();
	FD_ZERO(&_fd_set);
	std::map<long, Server>::iterator iter;
	for (iter = _servers.begin(); iter != _servers.end(); iter++)
		FD_SET(iter->first, &_fd_set);
	_endServer = TRUE;
}

/*************************************************************/
/* Loop waiting for incoming connects or for incoming data   */
/* on any of the connected sockets.                          */
/* Initialize the timeval struct to 3 minutes.  If no        */
/* activity after 3 minutes this program will end.           */
/*************************************************************/
void Cluster::waitingLoop() {
	_endServer = FALSE;
	do {
		fd_set reading_set;
		fd_set writing_set;
		struct timeval timeout = {};
		int ret = 0;

		while (ret == 0) {
			/**********************************************************/
			/* Call select() and wait for it to complete.   		  */
			/**********************************************************/
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			memcpy(&reading_set, &_fd_set, sizeof(_fd_set));
			FD_ZERO(&writing_set);
			std::vector<int>::iterator it;
			for (it = _ready.begin(); it != _ready.end(); it++)
				FD_SET(*it, &writing_set);
			std::cout << BLUE << "\rWaiting on a select" << RESET << std::flush;
			ret = select((int) _max_fd + 1, &reading_set,
						 &writing_set, NULL, &timeout);
		}
		if (ret > 0) {
			/**********************************************/
			/* Echo the data back to the client           */
			/**********************************************/
			sendingData(writing_set, reading_set);
			/**********************************************/
			/* Data was received                          */
			/**********************************************/
			receiveData(reading_set);
			/*************************************************/
			/* Accept all incoming connections that are      */
			/* queued up on the listening socket before we   */
			/* loop back and call select again.              */
			/*************************************************/
			acceptedIncomingConnection(reading_set);
		} else
			closeFailedConnection();
	} while (_endServer == FALSE);
}

void Cluster::clean() {
	std::map<long, Server>::iterator it;
	for (it = _servers.begin(); it != _servers.end(); it++)
		it->second.clean();
}

Cluster &Cluster::operator=(const Cluster &src) {
	_config = src._config;
	_servers = src._servers;
	_fd_set = src._fd_set;
	_fd_size = src._fd_size;
	_max_fd = src._max_fd;
	return (*this);
}

Cluster::Cluster() : _config(DEFAULT_CONFIG) {
}

Cluster::Cluster(const Cluster &src) {
	if (this != &src) {
		this->operator=(src);
	}
}

Cluster::~Cluster() {
}
