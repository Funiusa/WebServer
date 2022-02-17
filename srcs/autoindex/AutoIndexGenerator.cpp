#include "AutoIndexGenerator.hpp"

AutoIndexGenerator::AutoIndexGenerator() {
}

AutoIndexGenerator::AutoIndexGenerator(AutoIndexGenerator const &src) {
	this->operator=(src);
}

AutoIndexGenerator &AutoIndexGenerator::operator=(AutoIndexGenerator const &src) {
	(void) src;
	return *this;
}

AutoIndexGenerator::~AutoIndexGenerator() {
}

std::string AutoIndexGenerator::getPage(std::string path) {
	DIR *dir;
	int pos;
	struct stat s = {};
	std::string page;
	struct dirent *ent;
	std::string dirName;

	if ((dir = opendir((path).c_str())) != NULL) {
		if (path[path.size() - 1] == '/') {
			pos = (int) path.find_last_of('/');
			path.erase(pos);
		}
		pos = (int) path.find_last_of('/');
		dirName = path.substr(pos + 1, path.size()).append("/");
		std::vector<std::string> autoindex;
		while ((ent = readdir(dir)) != NULL) {
			autoindex.push_back(ent->d_name);
		}
		closedir(dir);
		page = "<!DOCTYPE html>\n"
			   "<html>\n<head>\n"
			   "<title>Index of "
			   "<link rel=\"icon\" type=\"image/x-icon\" href=\"images/favicon.ico\">"
			   + dirName + "</title>\n</head>\n"
						   "<body bgcolor='white'>\n"
						   "<h1 style='font-size:50px;'>Index of "
			   + dirName + "</h1>\n<hr>\n<pre>";

		for (int i = 0; i < (int) autoindex.size(); i++) {
			if (lstat((path + "/" + autoindex.at(i)).c_str(), &s) == 0) {
				if (autoindex.at(i) == ".")
					continue;
				if (S_ISDIR(s.st_mode)) {
					autoindex.at(i).append("/");
					page.append("\n<a href='");
					page.append(autoindex.at(i));
					page.append("'>");
					page.append(autoindex.at(i));
					page.append("</a>\n");
				} else {
					page.append("\n<a href='data:application/xml;charset=utf-8'");
					page.append(" download='");
					page.append(autoindex.at(i));
					page.append("'>");
					page.append(autoindex.at(i));
					page.append("</a>\n");
				}
			}
		}
		page.append("\n</pre>\n<hr>\n</body>\n</html>");
	} else
		std::cerr << RED << "\nError: could not open [" << path << "]\n"
				  << "Maybe file does not exist" << RESET << std::endl;
	return page;
}
