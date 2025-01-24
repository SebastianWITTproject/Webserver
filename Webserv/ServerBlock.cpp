#include "ServerBlock.hpp"

ServerBlock::ServerBlock()
	: socket_fd(),
		port(""),
		arr(),
		def(),
		host("")
{

}

ServerBlock::ServerBlock(Addrinfo& info)
	: socket_fd(info),
		port(""),
		arr(),
		def(),
		host("")
{

}

int	ServerBlock::get_fd() const
{
	return (socket_fd.get_fd());
}

void	ServerBlock::set_fd(int temp)
{
	socket_fd.set_fd(temp);
}

void	ServerBlock::set_port(const std::string &port)
{
	this->port = port;
}

int	ServerBlock::accept_connect()
{
	return(socket_fd.accept_connect());
}

void	ServerBlock::launch_serv()
{
	if (port.size() == 0 || host.size() == 0)
		throw(std::exception());
	Addrinfo	test(AF_INET, SOCK_STREAM, 0, AI_PASSIVE, port, host.c_str());
	Fsocket		sock(test);

	socket_fd = sock;
	socket_fd.init_connect(1);
}

void	ServerBlock::set_launched(bool val)
{
	socket_fd.set_launched(val);
}

void ServerBlock::add(const VirtualServ& serv)
{
	if (arr.find(serv.get_name()) != arr.end())
	{
		std::cerr << "[ERROR] two virtual serv have the same name" << std::endl;
		throw (std::exception());
	}
	arr[serv.get_name()] = serv; // const assign to non const problem ?
}

// copie partout pas opti
void	ServerBlock::set_default(const VirtualServ& serv)
{
	def = serv;
	add(serv);
}

std::string	ServerBlock::get_port() const
{
	return (port);
}

std::string	ServerBlock::get_host() const
{
	return (host);
}

void	ServerBlock::set_host(const std::string& temp)
{
	host = temp;
}

std::map<std::string, VirtualServ>::iterator	ServerBlock::end()
{
	return (arr.end());
}

VirtualServ* ServerBlock::find(const std::string& name, const std::string& t_port)//retourner un const virtualserv?
{
	std::map<std::string, VirtualServ>::iterator	it = arr.find(name);

	if (it != arr.end())
	{
		if (t_port.size() == 0 && it->second.get_port() == "80")
			return (&it->second);
		else if (t_port == it->second.get_port())
			return (&it->second);
	}
	std::cout << "t_port : " << t_port <<std::endl;
	std::cout << "port : " << port <<std::endl;
	std::cout << "host : " << host <<std::endl;
	std::cout << "name : " << name <<std::endl;
	if ((name == host || host == DEFAULT_IP) && t_port == port)
		return (&def);
	std::cout << "bUGGGGGGGGG" << std::endl;
	return (0);
}

VirtualServ* ServerBlock::unique()
{
	if (arr.size() == 1)
		return (&arr.begin()->second);
	return (0);
}

VirtualServ* ServerBlock::get_default()
{
	return (&def);
}