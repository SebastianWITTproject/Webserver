#ifndef MAIN_HPP
# define MAIN_HPP
# define SERVER_BLOCK 2
#include <poll.h>
#include <string>
#include <iostream>
#include <signal.h>
#include "Webserv/Webserv.hpp"
#include "Webserv/Addrinfo.hpp"
#include "Webserv/Fsocket.hpp"
#include "Webserv/VirtualServ.hpp"

int	parse_config(std::string name, Webserv& server);
#endif