#include "ConfigReader.hpp"

ConfigReader::ConfigReader() {
}

ConfigReader::ConfigReader(ConfigReader const &src) {
	if (this != &src)
		*this = src;
}

ConfigReader &ConfigReader::operator=(const ConfigReader &src) {
	if (this != &src)
		*this = src;
	return *this;
}

ConfigReader::~ConfigReader() {
}

vectorOfStrings ConfigReader::readFile(const std::string &filename) {
	std::string line;
	vectorOfStrings tmp;
	std::ifstream out;

	out.open(filename.c_str());
	if (!out.is_open()) {
		throw ConfigReader::FileNotFoundException();
	} else {
		while (getline(out, line))
			tmp.push_back(line);
	}
	out.close();

	vectorOfStrings file;
	for (unsigned int i = 0; i < tmp.size(); i++) {
		std::istringstream iss(tmp[i]);
		while (iss >> line)
			file.push_back(line);
	}
	return file;
}

const char *ConfigReader::FileNotFoundException::what() const throw() {
	return "Could not open configuration file";
}
