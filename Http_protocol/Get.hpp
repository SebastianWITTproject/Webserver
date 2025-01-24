#ifndef GET_HPP
#define GET_HPP
#include <string>
#include "Request.hpp"
#define CGI 1
#define AUTOINDEX 2
#define GETFILE 3

class Request;

class Get : public Request
{
	public :

		Get();
		Get(const std::string& target);
		~Get();
		int	response(int fd);
		std::string	type();
		int	type_code();
	private :
		//mettre cgi dans une classe a part
		int	send_file();
		int	send_header();
		std::fstream	file_to_send;
};

#endif