
#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "../webserv.hpp"

# include "Config.hpp"
# include "Server.hpp"
#include <poll.h>

class Cluster {

private:
	Config _config;
	fd_set _fd_set;
	long _max_fd;
	bool _endServer;
	unsigned int _fd_size;
	std::map<long, Server> _servers;
	std::map<long, Server *> _sockets;
	std::vector<int> _ready;
public:
	Cluster();

	Cluster(const Cluster &src);

	~Cluster();

	Cluster &operator=(const Cluster &src);

	void config(const std::string&);

	int createServer();

	void waitingLoop();

	void closeFailedConnection();

	void clean();

	void receiveData(fd_set reading_set);

	void sendingData(fd_set writing_set, fd_set reading_set);

	void acceptedIncomingConnection(fd_set reading_set);

};

#endif
