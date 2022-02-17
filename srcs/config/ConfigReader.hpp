#ifndef CONFIG_READER_HPP
# define CONFIG_READER_HPP

# include "../webserv.hpp"

class ConfigReader {
public:
	ConfigReader();

	ConfigReader(ConfigReader const &src);

	virtual    ~ConfigReader();

	ConfigReader &operator=(ConfigReader const &src);

	static vectorOfStrings readFile(const std::string &);

	class FileNotFoundException : public std::exception {
		virtual const char *what() const throw();
	};

};

#endif
