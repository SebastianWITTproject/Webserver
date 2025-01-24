#include "main.hpp"
#include "Exception.hpp"
#include "Webserv/VirtualServ.hpp"
#include "Webserv/ServerBlock.hpp"
#include <map>
#include <vector>
static int	search_block(std::fstream &file, std::string &line)
{
	std::size_t	pos = std::string::npos;
	std::string	sub;

	while (pos == std::string::npos && std::getline(file, line))
	{
		pos = line.find("server", 0);
		if (pos == std::string::npos && line.find_first_not_of(" \t") != std::string::npos)
			throw (ConfigException());
		else if (pos != std::string::npos && line.find_first_not_of(" \t") < pos)
			throw (ConfigException());
	}
	if (pos == std::string::npos)
		return (EOF);
	try
	{
		do
		{
			sub = line.substr(pos + 6);
			pos = sub.find_first_not_of(" \t");
			if (pos != std::string::npos)
			{
				if (sub[pos] == '{')
					return (SERVER_BLOCK);
				throw (ConfigException());
			}
			pos = -6;
		} while (std::getline(file, line));
	}
	catch (std::exception &e)
	{
		std::cerr << "exception catched" << std::endl;
	}
	// throw(EmptyFile());
	return (EOF);
}

static void parse_route(std::fstream &config, std::string &line, VirtualServ &server) {
    Route route;
    std::size_t pos = 0;
    std::size_t end = 0;
    std::string sub;


    while (std::getline(config, line)) {
        pos = line.find_first_not_of(" \t");
        if (pos == std::string::npos) {
            std::cerr << "Skipping empty or whitespace-only line.\n";
            continue;
        }


        if (line[pos] == '}') {
            break;
        }

        sub = line.substr(pos);
        end = sub.find_first_of(" \t;}");
        if (end == std::string::npos || sub[end] == '}') {
            std::cerr << "Error: Invalid line format in route block: " << line << "\n";
            throw std::exception();
        }


      	std::map<std::string, ParseFunction>::iterator it = Route::route_directives.find(sub.substr(0, end));
		if (it != Route::route_directives.end()) {
   	 std::cerr << "Found route directive: " << sub.substr(0, end) << "\n";
    	ParseFunction parse_func = it->second;
    	parse_func(sub.substr(end), route);
	 	}else {
    	std::cerr << "Error: Unknown route directive: " << sub.substr(0, end) << "\n";
		}


        if (sub.substr(sub.find(';') + 1).find_first_not_of(" \t") != std::string::npos) {
            std::cerr << "Error: Unexpected characters after directive in line: " << line << "\n";
            throw std::exception();
        }
    }


	const std::map<std::string, Route>& existing_routes = server.get_routes();
	if (existing_routes.find(route.get_location()) != existing_routes.end()) {
    std::cerr << "Error: Duplicate location detected: " << route.get_location() << "\n";
    throw std::exception();
	}

	 if (route.is_cgi_enabled() && route.get_upload_path().size() > 0) {
        std::cerr << "Error: A route cannot have both CGI enabled and an upload path set.\n";
        throw std::runtime_error("Route conflict: CGI and upload path cannot coexist.");
    }

    if (route.get_location() != "" && route.get_methods() != 0) {
        server.add_route(Route(route));
    } else {
        std::cerr << "Error: Invalid route. Must have a location and at least one allowed method.\n";
		throw std::exception();
    }


}
void config_server(std::fstream &config, std::string &line, VirtualServ &server) {

    std::map<std::string, ServerParseFunction>::iterator it;
    std::size_t pos;
    std::size_t end;
    std::string sub;


    while (true) {

        pos = line.find_first_of('{');
        if (pos != std::string::npos) {
            break;
        } else {
            std::cerr << "No opening brace found in this line, moving to next.\n";
        }
    }


    while (std::getline(config, line)) {
        pos = line.find_first_not_of(" \t");
        if (pos == std::string::npos) {
            continue;
        }




        if (line[pos] == '}') {
            break;
        }

        sub = line.substr(pos);
        end = sub.find_first_of(" \t;{");
        if (end == std::string::npos) {
            std::cerr << "Error: Invalid line format in server block: " << line << "\n";
            throw std::exception();
        }


        if (sub[end] == '{') {
            parse_route(config, line, server);
        } else {
            it = VirtualServ::server_elem.find(sub.substr(0, end));
            if (it == VirtualServ::server_elem.end()) {
                std::cerr << "Error: Unrecognized server directive: " << sub.substr(0, end) << "\n";
                throw std::exception();
            }

            std::cerr << "Processing server directive: " << sub.substr(0, end) << "\n";
            std::cerr << "Directive argument: " << sub.substr(end) << "\n";


            it->second(sub.substr(end), server);

        if (sub.substr(sub.find(';') + 1).find_first_not_of(" \t") != std::string::npos) {
            std::cerr << "Error: Unexpected characters after directive in line: " << line << "\n";
            throw std::exception();
        }
		}
    }

	if (server.get_name() == "" || server.get_port() == "" || server.get_protocol() == 0
		|| server.get_default_route().get_root() == "") {
        std::cerr << "Error: Server configuration incomplete. Missing one or more required elements that are:\n"
                  << " - server_name\n"
                  << " - listen\n"
                  << " - root\n"
                  << " - default\n"
                  << " - protocol\n";
        throw std::exception();
    }

    if (line.find('}') == std::string::npos) {
        std::cerr << "Error: Missing closing brace in server block.\n";
        throw std::exception();
    }


}

int	parse_config(std::string name, Webserv& server)
{
	std::vector<ServerBlock*>	temp_arr;
	std::fstream				config;
	std::string					line;
	int							stop = 0;

	config.open(name.c_str(), std::ofstream::in);
	if (config.fail())
	{
		std::cerr << "cannot open onfig file"<< std::endl;
		return (1);
	}
	while (search_block(config, line) != EOF)
	{
		VirtualServ					temp_serv;
		ServerBlock					temp_block;
		stop = 0;
		temp_block.set_fd(-1);
		temp_block.set_launched(0);
		config_server(config, line, temp_serv);
		for(unsigned long i = 0; i < temp_arr.size(); i++)
		{
			if (temp_arr[i]->get_port() == temp_serv.get_port() && temp_arr[i]->get_host() == temp_serv.get_host())
			{
				temp_arr[i]->add(VirtualServ(temp_serv));
				stop = 1;
				break;
			}
		}
		if (stop)
		{
			temp_block.set_fd(-1);
			continue;
		}
		temp_block.set_port(temp_serv.get_port());
		temp_block.set_host(temp_serv.get_host());
		temp_block.set_default(VirtualServ(temp_serv));
		temp_block.launch_serv();
		temp_arr.push_back(new ServerBlock(temp_block));
		temp_block.set_fd(-1);
	}
	server.add_serv(temp_arr);
	return (0);
}