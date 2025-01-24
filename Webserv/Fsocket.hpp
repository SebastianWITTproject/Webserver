#ifndef FSocket_HPP
# define FSocket_HPP
# include <unistd.h>
# include <sys/socket.h>
# include <exception>
# include <string>
# include <fcntl.h>
# include <string.h>
# include <iostream>
# include "Addrinfo.hpp"
# define LISTEN 100

// typedef union {
// 	struct sockaddr		addr;
// 	struct sockaddr_in	addr_in;
// }	u_sock;

class Fsocket
{
	private:
		int			fd;
		int			launched;
		Addrinfo	my_info;

	public:
		int		get_fd() const;
		void	set_fd(int val);
		void	set_launched(bool val);
		int		accept_connect();
		template <typename A>
		void	init_connect(const A& opt){
			if (launched || fd < 0)
				throw(std::exception());

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(A)) < 0)
				throw(std::exception());
			if (bind(fd, my_info.get_addr(), my_info.get_addrlen()) < 0)
				throw(std::exception());
			if (listen(fd, LISTEN))
				throw(std::exception());
			launched = 1;
		};
		Fsocket();
		Fsocket(Fsocket& to_copy);
		Fsocket(Addrinfo &info);
		void	operator=(Fsocket& to_copy);
		~Fsocket();
};
#endif