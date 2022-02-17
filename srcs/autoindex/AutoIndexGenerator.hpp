#ifndef AUTO_INDEX_GENERATOR_HPP
# define AUTO_INDEX_GENERATOR_HPP

# include "../webserv.hpp"

class AutoIndexGenerator {
public:
	AutoIndexGenerator();

	AutoIndexGenerator(AutoIndexGenerator const &src);

	virtual ~AutoIndexGenerator();

	AutoIndexGenerator &operator=(AutoIndexGenerator const &);

	static std::string getPage(std::string path);
};

#endif
