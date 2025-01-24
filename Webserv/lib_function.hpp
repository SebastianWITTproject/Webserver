#ifndef LIB_FUNCTION_HPP
#define LIB_FUNCTION_HPP
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <stdexcept>
# define GET 1
# define POST 2
# define DELETE 4
class Request;
int myascci(int c);
int alnum_path(int c);
bool is_directory(const std::string &path);
bool is_file(const std::string &path);
void check_directory_path(const std::string& path);
void check_file_path(const std::string& path);
std::string	get_value(const std::string& line, int (*f)(int));

#endif