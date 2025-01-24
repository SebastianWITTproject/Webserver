#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP
# include <string>
# include <map>
# include <iostream>
# include "VirtualServ.hpp"
# define DEFAULT_IP "0.0.0.0"
class VirtualServ;

class ServerBlock{

	public :

		ServerBlock();
		ServerBlock(Addrinfo& info);
		void											add(const VirtualServ& serv);
		std::map<std::string, VirtualServ>::iterator	end();
		VirtualServ*									find(const std::string& name, const std::string& t_port);
		VirtualServ*									unique();
		VirtualServ*									get_default();
		void											set_fd(int temp);
		void											set_host(const std::string& temp);
		void											set_default(const VirtualServ& serv);
		void											set_port(const std::string& port);
		void											set_launched(bool val);
		int												accept_connect();
		int												get_fd() const;
		std::string										get_port() const;
		std::string										get_host() const;
		void											launch_serv();

	private :

		Fsocket									socket_fd;
		std::string								port;
		std::map<std::string, VirtualServ>		arr;
		VirtualServ								def;
		std::string								host;
};
#endif