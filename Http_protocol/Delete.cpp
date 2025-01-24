#include "Delete.hpp"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Error.hpp"

Delete::Delete() : Request()
{

}


int Delete::response(int fd)
{

    std::cerr << "Handling DELETE request for target: " << target << std::endl;


    if (err)
        return err.trap_card_activate();

	//verifier si la methode est autorisee
     if (!(route.get_methods() & DELETE))
	{
		int	data = route.get_methods();
        return Error::handle_error(fd, serv, "405", 405, &data);
	}
    if (is_directory(target))
    {
        return Error::handle_error(fd, serv, "403", 403, 0);
    }

    if (access(target.c_str(), F_OK) != 0)
    {
        return Error::handle_error(fd, serv, "404", 404, 0);
    }

    if (unlink(target.c_str()) != 0)
    {
        return Error::handle_error(fd, serv, "403", 403, 0); // Check si 403 exist
    }


    std::string response = "HTTP/1.1 204 No Content\r\n";
    response.append("\r\n\r\n");

	int i = write(fd, response.c_str(), response.size());
    if (i == -1 || i == 0)
		return (CLOSE);

    return 1;
}


std::string	Delete::type()
{
	return ("DELETE");
}

Delete::~Delete()
{

}

int Delete::type_code()
{
	return (DELETE);
}