#include "webserv.hpp"
#include "Cluster.hpp"

int main(int argc, char **argv) {
	if (argc == 2) {
		Cluster cluster;

		try {
			cluster.config(argv[1]);
			if (cluster.createServer() == -1)
				return (1);
			cluster.waitingLoop();
			cluster.clean();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	} else {
		std::cerr << RED << "Invalid arguments"
				  << RESET << std::endl;
		return (1);
	}
	return (0);
}
