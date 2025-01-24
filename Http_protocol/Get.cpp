#include "Get.hpp"

//directory listing headers

#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

std::string generate_directory_listing(const std::string& directory_path, const std::string& request_path) {
    DIR* dir = opendir(directory_path.c_str());
    if (!dir) {
        throw std::runtime_error("Failed to open directory: " + directory_path);
    }

    std::ostringstream html;
    html << "<html><head><title>Directory Listing</title></head><body>";
    html << "<h1>Index of " << request_path << "</h1>";
    html << "<ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".") continue; // Skip current directory
        if (name == "..") {
            html << "<li><a href=\"../\">Parent Directory</a></li>";
            continue;
        }

 			std::string href = request_path;
 	       if (!href.empty() && href[href.size() - 1] != '/') {
            href += '/';
        }
        html << "<li><a href=\"" << href << name << "\">" << name << "</a></li>";
    }

    html << "</ul></body></html>";
    closedir(dir);
    return html.str();
}

Get::Get(): Request()
{

}

Get::~Get()
{

}

static std::string find_file_size(std::fstream& file)
{
	std::streampos temp = file.tellg();

	file.seekg(0, file.beg);
	std::ifstream::pos_type begin = file.tellg();
	file.seekg(0, file.end);
	std::stringstream	stream;
	stream << (static_cast<long>(file.tellg() - begin));
	file.seekg(temp);
	return (stream.str());
}

//utiliser map pour opti la rechherche
static std::string find_content_type(std::string& file)
{
	if (file.find(".jpg") != std::string::npos)
		return("image/jpeg\r\n");
	else if (file.find(".html") != std::string::npos)
		return("text/html\r\n");
	else if (file.find(".css") != std::string::npos)
		return("text/css\r\n");
	else if (file.find(".js") != std::string::npos)
		return("application/javascript\r\n");
	else if (file.find(".png") != std::string::npos)
		return("image/png\r\n");
	else if (file.find(".eot") != std::string::npos)
		return("application/vnd.ms-fontobject\r\n");
	else if (file.find(".svg") != std::string::npos)
		return("image/svg+xml\r\n");
	else if (file.find(".woff") != std::string::npos)
		return("font/woff\r\n");
	else if (file.find(".woff2") != std::string::npos)
		return("font/woff2\r\n");
	return ("");
}

static void make_header(std::string& header, std::string& content_length, std::string& content_type)
{
	header = "HTTP/1.1 200 OK\r\n";
	header.append("Content-Length: ").append(content_length);
	if (content_type.size() != 0)
		header.append("Content-Type: ").append(content_type);
	header.append("\r\n");
}

int	Get::send_file()
{
	file_to_send.read(&buff[0], BUFF_SIZE);
	if (file_to_send.rdstate() & std::fstream::badbit)
	{
		first = 0;
		file_to_send.close();
		return (1);
	}
	int i = write(fd, buff, file_to_send.gcount());
	if ( i == -1 || i == 0)
		return (CLOSE);
	if (file_to_send.gcount() < BUFF_SIZE)
	{
		first = 0;
		file_to_send.close();
		return (1);
	}
	return (0);
}

int	Get::send_header()
{
	std::string	header;
	std::string path = target;

	if (access(path.c_str(), F_OK ))
		return (Error::handle_error(fd, serv, "404", 404, 0));//a check
	if (access(path.c_str(), R_OK))
		return (Error::handle_error(fd, serv, "403", 403, 0));
	file_to_send.open(path.c_str(), std::fstream::in | std::ios::binary);
	if (!file_to_send.is_open())
		return (Error::handle_error(fd, serv, "403", 403, 0));//a check
	content_length = find_file_size(file_to_send).append("\r\n");
	content_type = find_content_type(target);
	make_header(header, content_length, content_type);
	int i = write(fd, header.c_str(), header.size());
	if (i == 0 || i == -1)
		return (CLOSE);
	return (0);
}

int	Get::response(int fd)
{
	if (!first)
	{
		first = 1;
		std::cerr << "heho: " <<  target << std::endl;
		if (err)
			return (err.trap_card_activate());
		if (!(route.get_methods() & GET))
		{
			int data = route.get_methods();
			return (Error::handle_error(fd, serv, "405", 405, &data));//a changer en method not allowed
		}
		if (target.find(".cgi") != std::string::npos)
		{
			if (!route.is_cgi_enabled())
				return (Error::handle_error(fd, serv, "403", 403, 0));
			method = CGI;
			return (set_up_cgi(fd));
		}
		else if (is_directory(target))
		{
			if (route.get_default().size() > 0)
			{
				method = GETFILE;
				target.append(route.get_default());
				std::cerr << "New target : " << target << std::endl;
				if (access(target.c_str(), F_OK | R_OK) < 0)
					return	(Error::handle_error(fd, serv, "403", 403, 0));
				return (send_header());
			}
			else if (route.is_autoindex())
				method = AUTOINDEX;
			else
				return (Error::handle_error(fd, serv, "403", 403, 0));
		}
		else
		{
			method = GETFILE;
			return (send_header());
		}
	}
	else
	{
		switch (method){
			case GETFILE:
				return (send_file());
			case AUTOINDEX:
				try {

                    std::string html = generate_directory_listing(target, url);


                    std::string header = "HTTP/1.1 200 OK\r\n";
                    std::stringstream ss;
	             	 ss << html.size();
					header.append("Content-Length: ").append(ss.str()).append("\r\n");

                    header.append("Content-Type: text/html\r\n\r\n");


					header += html;
					int i = write(fd, header.c_str(), header.size());
                    if (i == -1 || i == 0) {
                        return CLOSE;
                    }
                    first = 0;
                } catch (const std::exception& e) {
                    return Error::handle_error(fd, serv, "500", 500, 0);
                }
                return 1;
				break;
			case CGI:
				return (cgi_handler(fd));
				break;
		}
	}
	return (0);
}

std::string	Get::type()
{
	return ("GET");
}

int Get::type_code()
{
	return (GET);
}

