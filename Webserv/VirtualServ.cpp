# include "VirtualServ.hpp"
#include <iostream>
#include <sys/stat.h>

std::map<std::string, ServerParseFunction> VirtualServ::server_elem = VirtualServ::init_static_elem();

void	VirtualServ::server_var(const std::string &line, VirtualServ &serv)
{
	std::string	temp = get_value(line, isalnum);
	serv.set_name(temp);
	std::cerr << "/" << temp << "/" << std::endl;
}

void VirtualServ::add_route(const Route& route) {
     routes[route.get_location()] = route;
}

const std::map<std::string, Route>& VirtualServ::get_routes() const {
    return routes;
}

void	VirtualServ::listen_var(const std::string &line, VirtualServ &serv)
{
	std::string	temp = get_value(line, isdigit);
	serv.set_port(temp);
}

void	VirtualServ::host_var(const std::string &line, VirtualServ &serv)
{
	std::string	temp = get_value(line, myascci);
	serv.set_host(temp);
}

void	VirtualServ::root_var(const std::string& line, VirtualServ &serv)
{

	std::string	temp = get_value(line, alnum_path);
	std::cerr << "entering root- " << std::endl;
	check_directory_path(temp);
	serv.set_root(temp);

}

void	VirtualServ::default_var(const std::string& line, VirtualServ& serv)
{
	std::string	temp = get_value(line, myascci);
		serv.set_default(temp);

}

void	VirtualServ:: error_var(const std::string &line, VirtualServ &serv)
{
	std::string	temp = get_value(line, myascci);
	serv.set_error(temp);
}

void	VirtualServ::size_var(const std::string &line, VirtualServ &serv)
{
	std::string	temp = get_value(line, myascci);
	if (temp.size() > 10)
	{
		std::cout << "body size can not exceed 20mo" << std::endl;
		throw(std::exception());
	}
	float f = std::atof(temp.c_str());
	if (f <= 0)
	{
		std::cout << "body size can not be negative" << std::endl;
		throw(std::exception());
	}
	if (f > 1000000)
	{
		std::cout << "body size can not exceed 1mo" << std::endl;
		throw(std::exception());
	}
	serv.set_size(static_cast<int>(f));
}

void VirtualServ::protocol_var(const std::string& line, VirtualServ& serv)
{
    std::string temp = get_value(line, myascci);

    if (temp.find("GET") != std::string::npos)
        serv.accepted_protocol |= GET;
    if (temp.find("POST") != std::string::npos)
       serv.accepted_protocol |= POST;
    if (temp.find("DELETE") != std::string::npos)
        serv.accepted_protocol |= DELETE;

    std::cerr << "protocole : " << serv.accepted_protocol << std::endl;
}

std::map<std::string, ServerParseFunction> VirtualServ::init_static_elem()
{
	std::map<std::string, ServerParseFunction> ret;

	ret["server_name"] = VirtualServ::server_var;
	ret["listen"] = VirtualServ::listen_var;
	ret["root"] = VirtualServ::root_var;
	ret["default"] = VirtualServ::default_var;
	ret["protocol"] = VirtualServ::protocol_var;
	ret["host"] = VirtualServ::host_var;
	ret["error_page"] = VirtualServ::error_var;
	ret["body_size"] = VirtualServ::size_var;
	return (ret);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VirtualServ::VirtualServ()
	: port(""),
		server_name(""),
		root(""),
		host ("0.0.0.0"),
		error_page (""),
		body_size (0),
		default_page(""),
		default_route(),
		accepted_protocol(0),
		routes()
{

}

VirtualServ::~VirtualServ()
{

}

// void	VirtualServ::operator=(VirtualServ& to_copy)
// {
// 	socket_fd = to_copy.socket_fd;
// 	connection_fd = to_copy.connection_fd;
// 	port = to_copy.port;
// 	server_name = to_copy.server_name;
// }

//verifier ce qui se passe si fail de construction de socket_fd

void	VirtualServ::set_default(const std::string& line)
{
	this->default_route.set_default(line);
}


void	VirtualServ::set_name(const std::string &name)
{
	this->server_name = name;
}

void	VirtualServ::set_host(const std::string &host)
{
	this->host = host;
}

void	VirtualServ::set_error(const std::string &error)
{
	this->error_page = error;
}

void	VirtualServ::set_size(int size)
{
	this->body_size = size;
}

void	VirtualServ::set_port(const std::string &name)
{
	this->port = name;
}

void	VirtualServ::set_root(const std::string &root)
{
	this->default_route.set_root(root);
	this->default_route.set_location("/");
}

const Route&	VirtualServ::get_default_route() const
{
	return (default_route);
}

std::string	VirtualServ::get_root() const
{
	return (root);
}

std::string	VirtualServ::get_name() const
{
	return (server_name);
}

std::string	VirtualServ::get_port() const
{
	return (port);
}

std::string	VirtualServ::get_error() const
{
	return (error_page);
}

std::size_t VirtualServ::get_size() const
{
	return (body_size);
}

std::string	VirtualServ::get_host() const
{
	return (host);
}

int VirtualServ::get_protocol() const
{
	return (accepted_protocol);
}