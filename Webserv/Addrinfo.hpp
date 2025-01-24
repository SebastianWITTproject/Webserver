#ifndef ADDRINFO_HPP
# define ADDRINFO_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <stdlib.h>

class Addrinfo
{
	public :

		Addrinfo();
		Addrinfo(int family, int socktype, int protocol, int flags, const std::string &port, const char *ip);
		Addrinfo(Addrinfo& to_copy);
		~Addrinfo();
		int						get_family() const;
		int						get_protocol() const;
		int						get_socktype() const;
		int						get_flags() const;
		struct sockaddr*		get_addr();
		socklen_t				get_addrlen() const;
		std::string				get_port() const;
		void	operator=(Addrinfo& to_copy);

	private :
		struct addrinfo	hints;
		struct addrinfo	*res;
		std::string		port;
		std::string		ip;
};

#endif