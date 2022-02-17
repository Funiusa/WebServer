#ifndef WEBSERV_HPP
#define WEBSERV_HPP


#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <limits>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <utility>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

# define RECV_SIZE 65536
# define CGI_BUFSIZE 65536
#define MAX_CONNECTIONS 1000

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define RESET "\033[0m"

# ifndef OUT
#  define OUT 1
# endif
# ifndef TIME
#  define TIME 0
# endif

# define parseMap std::map<std::string, void (ConfigServer::*)(vectorOfStrings)>

typedef std::vector<std::string> vectorOfStrings;

class ConfigServer;

class RequestConfig;

typedef struct s_listen {
	unsigned int host;
	int port;
} t_listen;

std::string readKey(char *line); //Deprecated
std::string readKey(const std::string &line);

std::string readValue(char *line); //Deprecated
std::string readValue(const std::string &line);

std::string &strip(std::string &str, char c);

std::vector<std::string> split(const std::string &str, char c);

std::string &to_lower(std::string &str);

std::string &to_upper(std::string &str);

std::string &capitalize(std::string &str);

std::string to_string(size_t n);

int checkEnd(const std::string &str, const std::string &end);

int checkStart(const std::string &str, const std::string &end);

int countSubstr(const std::string &str, const std::string &sub);

std::string &pop(std::string &str);

int pathIsFile(const std::string &path);

std::string removeSlashes(const std::string &str);

unsigned int strToIp(std::string strIp);

void timeit(const std::string &breakpoint);

//bool compare_langs(const std::pair<std::string, float> first, const std::pair<std::string, float> second);


#endif
