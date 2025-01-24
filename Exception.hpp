#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP
# include <exception>

class ConfigException : public std::exception
{
	public :
		const char*	what() const throw();
};

class EmptyFile : public std::exception
{
	public :
		const char*	what() const throw();
};
#endif