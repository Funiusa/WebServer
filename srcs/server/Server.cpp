#include "Server.hpp"
#include "Socket.cpp"


int Server::createListeningSocket() {
	if (socketDescriptor() == -1 || setSockOpt() == -1
		|| nonBlockSocket() == -1 || socketBind() == -1
		|| socketListener() == -1)
		return -1;
	return (0);
}

/**********************************************/
/* Accept each incoming connection.  If       */
/* accept fails with EWOULDBLOCK, then we     */
/* have accepted all of them.  Any other      */
/* failure on accept will cause us to end the */
/* server.                                    */
/**********************************************/
long Server::acceptIncomingConnection() {
	long new_sd;

	new_sd = accept(_fd, NULL, NULL);
	if (new_sd < 0) {
		if (errno != EWOULDBLOCK) {
			perror("  accept() failed");
		}
		return (-1);
	}
	if (new_sd > 0) {
		if (fcntl((int) new_sd, F_SETFL, O_NONBLOCK) < 0) {
			closeSockFD((int) new_sd);
			return (-1);
		}
		_requests.insert(std::make_pair(new_sd, ""));
	}

	return (new_sd);
}

// Getting a file in chunks if it so big
void Server::processChunk(long socket) {
	std::string head = _requests[socket].substr(0, _requests[socket].find("\r\n\r\n"));
	std::string chunks = _requests[socket].substr(_requests[socket].find("\r\n\r\n") + 4, _requests[socket].size() - 1);
	std::string subChunk = chunks.substr(0, 100);
	std::string body;
	size_t i = 0;

	long chunkSize = strtol(subChunk.c_str(), NULL, 16);

	while (chunkSize) {
		i = chunks.find("\r\n", i) + 2;
		body.append(chunks.substr(i, chunkSize));
		i += chunkSize + 2;
		subChunk = chunks.substr(i, 100);
		chunkSize = strtol(subChunk.c_str(), NULL, 16);
	}
	_requests[socket] = head + "\r\n\r\n" + body + "\r\n\r\n";
}

// Main server process
void Server::process(long socket, Config &conf) {
	RequestConfig requestConf;
	Response response;

	// if request have chunked transfer
	if (_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos &&
		_requests[socket].find("Transfer-Encoding: chunked") < _requests[socket].find("\r\n\r\n"))
		this->processChunk(socket);
	if (_requests[socket].size() < 1000) // show request on console
		std::cout << "\nRequest: \n"
				  << "[" << YELLOW << _requests[socket] << RESET << "]" << std::endl;
	else
		std::cout << "\nRequest: \n"
				  << "[" << YELLOW << _requests[socket].substr(0, 1000)
				  << _requests[socket].substr(_requests[socket].size() - 10, 15)
				  << RESET << "]" << std::endl;

	if (!_requests[socket].empty()) {
		Request request(_requests[socket]);
		if (request.getRet() != 200)
			request.setMethod("GET");
		requestConf = conf.getConfigForRequest(this->_listen,
											   request.getPath(),
											   request.getHeaders().at("Host"),
											   request.getMethod(), request);
		// Response, choose the method
		response.answerToClient(request, requestConf);
		_requests.erase(socket); // Clean request
		_requests.insert(std::make_pair(socket, response.getResponse()));
	}
}


int Server::receiveIncomingData(long socket) {
	char buffer[RECV_SIZE] = {0};
	ssize_t rc = recv((int) socket, buffer, RECV_SIZE - 1, 0);

	if (rc == 0 || rc == -1) {
		if (!rc)
			std::cout << "\rConnection was closed by client\n" << std::endl;
		else {
			std::cout << "\rReceive error, closing connection\n" << std::endl;
		}
		this->closeSockFD((int) socket);
		return (-1);
	}

	_requests[socket] += std::string(buffer);
	size_t i = _requests[socket].find("\r\n\r\n");
	if (i != std::string::npos) {
		if (_requests[socket].find("Content-Length: ") == std::string::npos) {
			if (_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos) {
				if (checkEnd(_requests[socket], "0\r\n\r\n") == 0)
					return (0);
				else
					return (1);
			} else
				return (0);
		}

		size_t len = std::strtol(_requests[socket].substr(_requests[socket].find("Content-Length: ") + 16, 10).c_str(),
								 NULL, 10);
		if (_requests[socket].size() >= len + i + 4)
			return (0);
		else
			return (1);
	}
	return (1);
}

int Server::echoDataToClient(long socket) {
	static std::map<long, size_t> sent;

	if (sent.find(socket) == sent.end())
		sent[socket] = 0;

	if (OUT && sent[socket] == 0) {
		if (_requests[socket].size() < 1000)
			std::cout << "\rResponse: " << std::endl << "[" << GREEN << _requests[socket]
					  << RESET << "]\n" << std::endl;
		else
			std::cout << "\rResponse: " << std::endl << "[" << GREEN
					  << _requests[socket].substr(0, 1000)
					  << _requests[socket].substr(_requests[socket].size() - 10, 15)
					  << RESET << "]\n" << std::endl;
	}

	std::string str = _requests[socket].substr(sent[socket], RECV_SIZE);
	ssize_t ret = send((int) socket, str.c_str(), str.size(), 0);
	if (ret == -1 || ret == 0) {
		this->closeSockFD((int) socket);
		sent[socket] = 0;
		return (-1);
	} else {
		sent[socket] += ret;
		if (sent[socket] >= _requests[socket].size()) {
			_requests.erase(socket);
			sent[socket] = 0;
			return (0);
		} else
			return (1);
	}
}

void Server::closeSockFD(int socket) {
	if (socket > 0)
		close(socket);
	_requests.erase(socket);
}

void Server::clean() {
	if (_fd > 0)
		close(_fd);
	_fd = -1;
}

long Server::getFD() const {
	return _fd;
}

Server &Server::operator=(const Server &src) {
	_listen = src._listen;
	_fd = src._fd;
	_addr = src._addr;
	return (*this);
}

Server::Server() {
}


Server::Server(const t_listen &listen) {
	_fd = -1;
	_listen = listen;
}

Server::Server(const Server &src) {
	this->operator=(src);
}

Server::~Server() {
}
