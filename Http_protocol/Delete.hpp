#ifndef DELETE_HPP
#define DELETE_HPP
#include <string>
#include "Request.hpp"

class Request;
class Delete : public Request
{
	public :
		Delete();
		Delete(const std::string& target);
		~Delete();
		int		response(int fd);
		std::string	type();
		int	type_code();
	private :

};
#endif