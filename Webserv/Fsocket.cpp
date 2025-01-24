#include "Fsocket.hpp"


Fsocket::Fsocket() : fd(-1), launched(0), my_info()
{

}

Fsocket::Fsocket(Addrinfo& info) : launched(0), my_info(info)
{
	fd = socket(my_info.get_family(), my_info.get_socktype() , my_info.get_protocol());
	if (fd < 0)
		throw(std::exception());
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		throw(std::exception());
}

Fsocket::Fsocket(Fsocket& to_copy) : fd(to_copy.fd), launched(to_copy.launched), my_info(to_copy.my_info)
{
	to_copy.fd = -1;
}

Fsocket::~Fsocket()
{
	if (fd >= 0)
		close(fd);
}

int	Fsocket::accept_connect()
{
	if (!launched)
		throw(std::exception());
	int			ret;
	// socklen_t	sock;
	// u_sock		addr;

	ret = accept(fd, 0, 0);

	return (ret);
}

void	Fsocket::set_fd(int temp)
{
	fd = temp;
}

void	Fsocket::set_launched(bool val)
{
	launched = val;
}

int	Fsocket::get_fd() const
{
	return (fd);
}

void Fsocket::operator=(Fsocket& to_copy)
{
	if (fd >= 0)
		close(fd);
	fd = to_copy.fd;
	my_info = to_copy.my_info;
	to_copy.fd = -1;
}
