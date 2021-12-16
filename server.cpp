
#include <sstream>
#include "server.hpp"
#include "request.hpp"
#include "Response.hpp"

#define MAX_CONNECTION 1000

Server::Server(int port)
: _rc(1),_listenSD(-1), _port(port), _nfds(1), _endServer(FALSE),
_currentSize(0), _newSd(-1), _compressArray(FALSE) {

}

Server::Server()
: _rc(1),_listenSD(-1), _port(8080), _nfds(1), _endServer(FALSE),
			_currentSize(0), _newSd(-1), _compressArray(FALSE) {
}

Server::~Server() {

}


bool Server::socketInit() {
	return true;
}

void Server::setListenSd(int sd) {
	this->_listenSD = sd;
}

/*************************************************************/
/* Initialize the pollfd structure                           */
/* Set up the initial listening socket                  	 */
/* pollfd structure                                  */
/*************************************************************/
void Server::pollInit() {

	memset(this->_fds, 0 , sizeof(this->_fds));
	this->_fds[0].fd = this->_listenSD;
	this->_fds[0].events = POLLIN;
	_timeout = (3 * 60 * 1000);

}
/***********************************************************/
/* Call poll() and wait 3 minutes for it to complete.      */
/* Check to see if the poll call failed.                   */
/* Initialize the timeout to 3 minutes. If no              */
/* activity after 3 minutes this program will end.         */
/* timeout value is based on milliseconds.                 */
/* Check to see if the 3 minute time out expired.          */
/***********************************************************/
void Server::pollWait() {


	this->_rc = poll(this->_fds, this->_nfds, _timeout);
	std::cout << "Waiting on poll()..." << std::endl;
	if (this->_rc < 0) {
		myErrorException("poll() failed");
	}
	if (this->_rc == 0) {
		myErrorException("poll() timed out.  End program.");
	}
}



/*****************************************************/
/* Accept each incoming connection. If               */
/* accept fails with EWOULDBLOCK, then we            */
/* have accepted all of them. Any other              */
/* failure on accept will cause us to end the        */
/* server.                                           */
/*****************************************************/
int Server::acceptIncomingConnections() {
	this->_newSd = accept(this->_listenSD, NULL, NULL);
	if (this->_newSd < 0) {
		if (errno != EWOULDBLOCK) {
			this->_endServer = TRUE;
			std::cout << "accept() failed" << std::endl;
			return (1);
		}
	}
	return (0);
}

/*****************************************************/
/* Add the new incoming connection to the            */
/* pollfd structure                                  */
/*****************************************************/
void Server::addNewIncomingConnection() {

	std::cout << "  New incoming connection - " << this->_newSd << std::endl;
	this->_fds[this->_nfds].fd = this->_newSd;
	this->_fds[this->_nfds].events = POLLIN;
	this->_nfds++;
}

/*************************************************************/
/* Loop waiting for incoming connects or for incoming data   */
/* on any of the connected sockets.                          */
/*************************************************************/
void Server::start() {
	do {
		pollWait();
		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		this->_currentSize = this->_nfds;

		/*********************************************************/
		/* Loop through to find the descriptors that returned    */
		/* POLLIN and determine whether it's the listening       */
		/* or the active connection.                             */
		/*********************************************************/
		for (int i = 0; i < this->_currentSize; i++) {
			fcntl(this->_fds[i].fd, F_SETFL, O_NONBLOCK);
			if (this->_fds[i].revents == 0)
				continue;
			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
			if (this->_fds[i].revents != POLLIN) {
				std::cout << "Revents " << this->_fds[i].revents << std::endl;
				this->_endServer = TRUE;
				std::cout << "Error revents" << std::endl;
				break ;
			}
			if (this->_fds[i].fd == this->_listenSD) {
				/*******************************************************/
				/* Listening descriptor is readable.                   */
				/*******************************************************/
				std::cout << "  Listening socket is readable" << std::endl;
				/*******************************************************/
				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				/*******************************************************/
				do {

					if (acceptIncomingConnections())
						break ;
					addNewIncomingConnection();
					/*****************************************************/
					/* Loop back up and accept another incoming          */
					/* connection                                        */
					/*****************************************************/
				} while (this->_newSd != -1);
			}

			/*********************************************************/
			/* This is not the listening socket, therefore an        */
			/* existing connection must be readable                  */
			/*********************************************************/
			else {
				std::cout << "Descriptor " << this->_fds[i].fd << " is readable" << std::endl;
				this->_closeConnection = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/
				do {
					Request req;
					if (req.receiveDate(this->_fds[i].fd))
						break ;
					req.readRequest();
					std::cout << "Type: " << req.get_type() << "; Uri: " << req.get_uri() << std::endl;

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					Response resp;
					resp.getHtmlData(req.get_uri());
					if (resp.sandResponse(this->_fds[i].fd))
						break ;

				} while (TRUE);
				/*******************************************************/
				/* If the _closeConn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (this->_closeConnection) {
					closeConnection(this->_fds[i].fd);
				}
			} /* End of existing connection is readable             */

		} /* End of loop through pollable descriptors              */


		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if (this->_compressArray)
			compressFdArray();

	} while (this->_endServer == FALSE); /* End of serving running.    */

/*************************************************************/
/* Clean up all of the sockets that are open                 */
/*************************************************************/
	cleanUpAllOpenSockets();
}

void Server::closeConnection(int fd) {
	close(fd);
	fd = -1;
	this->_compressArray = TRUE;
}


void Server::compressFdArray() {
	this->_compressArray = FALSE;
	for (int i = 0; i < this->_nfds; i++)
	{
		if (this->_fds[i].fd == -1) {
			for (int j = i; j < this->_nfds - 1; j++) {
				this->_fds[j].fd = this->_fds[j + 1].fd;
			}
			i--;
			this->_nfds--;
		}
	}
}

void Server::cleanUpAllOpenSockets() {

	for (int i = 0; i < this->_nfds; i++)
	{
		if (this->_fds[i].fd >= 0)
			close(this->_fds[i].fd);
	}
}

const char *Server::myErrorException::what() const throw() {
	return this->_errorString.c_str();
}
