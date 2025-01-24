#include "Exception.hpp"

const char*	ConfigException::what() const throw()
{
	return ("problem with parsing file");
}

const char*	EmptyFile::what() const throw()
{
	return ("config file empty");
}