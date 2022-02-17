#include "CgiHandler.hpp"
#include "Response.hpp"
#include <cstring>

CgiHandler::CgiHandler(Request &request, RequestConfig &config) :
		_body(request.getBody()) {
// body may be empty or not
// this is the filled page with some client changes
// in subject tester we return the body with some text
// in real world we return the page with some different aspects
	this->fillEnvReq(request, config);
}

CgiHandler::CgiHandler(CgiHandler const &src) {
	this->operator=(src);
}

CgiHandler::~CgiHandler() {
}

CgiHandler &CgiHandler::operator=(CgiHandler const &src) {
	if (this != &src) {
		this->_body = src._body;
		this->_env = src._env;
	}
	return *this;
}

void CgiHandler::fillEnvReq(Request &request, RequestConfig &config) {
	std::map<std::string, std::string> headers = request.getHeaders();
	if (headers.find("Auth-Scheme") != headers.end() && !headers["Auth-Scheme"].empty())
		this->_env["AUTH_TYPE"] = headers["Authorization"];

	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = config.getPath();
	this->_env["SCRIPT_FILENAME"] = config.getPath();

	this->_env["REQUEST_METHOD"] = request.getMethod();
	this->_env["CONTENT_LENGTH"] = to_string(this->_body.length());
	this->_env["CONTENT_TYPE"] = headers["Content-Type"];
	this->_env["PATH_INFO"] = request.getPath();
	this->_env["PATH_TRANSLATED"] = request.getPath();
	this->_env["QUERY_STRING"] = request.getQuery();

	this->_env["REMOTE_ADDR"] = to_string(config.getHostPort().host);
	this->_env["REMOTE_IDENT"] = headers["Authorization"];
	this->_env["REMOTE_USER"] = headers["Authorization"];
	this->_env["REQUEST_URI"] = request.getPath() + request.getQuery();
	if (headers.find("Hostname") != headers.end())
		this->_env["SERVER_NAME"] = headers["Hostname"];
	else
		this->_env["SERVER_NAME"] = this->_env["REMOTE_ADDR"];

	this->_env["SERVER_PORT"] = to_string(config.getHostPort().port);
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "Nonginx/1.0";

	this->_env.insert(config.getCgiParam().begin(), config.getCgiParam().end());
}

char **CgiHandler::envStringMap() const {
	char **env = new char *[this->_env.size() + 1];
	int j = 0;
	std::map<std::string, std::string>::const_iterator it;
	for (it = this->_env.begin(); it != this->_env.end(); it++) {
		std::string elem = it->first + "=" + it->second;
		env[j] = new char[elem.size() + 1];
		env[j] = strcpy(env[j], (const char *) elem.c_str());
		j++;
	}
	env[j] = NULL;
	return env;
}

std::string CgiHandler::executeCgi(const std::string &scriptName) {
	pid_t pid;
	int saveStdin;
	int saveStdout;
	char **env;
	std::string newBody;

	try {
		env = this->envStringMap();
	}
	catch (std::bad_alloc &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);
	FILE *fIn = tmpfile();
	FILE *fOut = tmpfile();
	int fdIn = fileno(fIn);
	int fdOut = fileno(fOut);


	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1) {
		std::cerr << RED << "Fork crashed." << RESET << std::endl;
		return ("Status: 500\r\n\r\n");
	} else if (!pid) {
		char *const *nullptr = NULL;
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(scriptName.c_str(), nullptr, env);
		std::cerr << RED << "Execve failed" << RESET << std::endl;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	} else {
		char buffer[CGI_BUFSIZE] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		int ret = 1;
		while (ret > 0) {
			memset(buffer, 0, CGI_BUFSIZE);
			ret = (int) read(fdOut, buffer, CGI_BUFSIZE - 1);
			newBody += buffer;
		}
	}

	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close((int) fdIn);
	close((int) fdOut);
	close(saveStdin);
	close(saveStdout);

	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	if (!pid)
		exit(0);
	return (newBody);
}
