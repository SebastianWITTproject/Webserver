#include "lib_function.hpp"

int myascci(int c)
{
	if (c != '}' && c != ';' && isascii(c))
		return (1);
	return (0);
}
int alnum_path(int c)
{
	if (c == '/' || c== '.'|| isalnum(c))
		return (1);
	return (0);
}

bool is_directory(const std::string &path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) == -1)
		return (0);
    return S_ISDIR(path_stat.st_mode);
}

bool is_file(const std::string &path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) == -1)
		return (0);
    return S_ISREG(path_stat.st_mode);
}

void check_directory_path(const std::string& path) {


    std::string temp = path;



    if (!is_directory(temp)) {
        throw std::runtime_error("Path is not a valid directory: " + temp);
    }


    if (access(temp.c_str(), R_OK | W_OK | X_OK) != 0) {
        throw std::runtime_error("Insufficient permissions for directory: " + temp);
    }
}

void check_file_path(const std::string& path) {

    std::string temp = path;


    if (!is_file(temp)) {
        throw std::runtime_error("Path is not a valid file: " + temp);
    }


    if (access(temp.c_str(), R_OK | W_OK) != 0) {
        throw std::runtime_error("Insufficient permissions for file: " + temp);
    }
}

std::string	get_value(const std::string& line, int (*f)(int))
{
	int			j = 0;
	std::string	sub;
	std::size_t	pos = line.find_first_not_of(" \t");
	std::size_t	temp = pos;

	if (pos == std::string::npos)
		throw(std::exception());
	while (f(line[pos + j]) && pos + j < line.size())
		j++;
	if (pos + j >= line.size())
		throw(std::exception());
	sub = line.substr(pos + j);
	pos = sub.find_first_not_of(" \t");
	if (pos == std::string::npos || sub[pos] != ';')
		throw(std::exception());
	return (line.substr(temp, j));
}