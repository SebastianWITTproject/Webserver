#ifndef POST_HPP
#define POST_HPP
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include "Request.hpp"
#define CGI 1
#define AUTOINDEX 2
#define GETFILE 3
#define UPLOAD 4
class Request;

class Post : public Request
{
	public :
		Post();
		Post(const std::string& target);
		~Post();
		int	response(int fd);
		std::string	type();
		int	type_code();
		std::string handle_multipart(int fd);
		int handle_upload(int fd);
	private :

		int method;

};

#endif