#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <unistd.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <cstdlib>
#include "../Webserv/Route.hpp"
#include "../Webserv/VirtualServ.hpp"
#include "Error.hpp"
#include "Http_header.hpp"
#include "../Webserv/lib_function.hpp"
# define CLOSE 2
#define BUFF_SIZE 10000
# define PROTOCOLE "Http/1.1"
# define GATEWAY "CGI/1.1"

class VirtualServ;
class ServerBlock;
class Get;

int	handle_error(int fd, const VirtualServ* server, std::string error_code, int error);

class Test : public std::exception
{
	public :
		const char*	what() const throw();
	private :

};

class Request
{
	public :

		Request();
		virtual ~Request();
		virtual int			response(int fd) = 0;
		virtual std::string						type() = 0;
		virtual int								type_code() = 0;

		std::string							get_location() const;
		Error&								get_error();
		std::string							get_status() const;
		std::string							get_content_type() const;
		std::string							get_protocole_version() const;
		std::string							get_content_length() const;
		std::string							get_target() const;
		std::string							get_upload() const;
		std::string							get_auth() const;
		std::string							get_url() const;
		std::string							get_boundary() const;
		std::string							get_host() const;
		std::string&						get_redirec();
		const std::string&					get_body() const;
		Route&								get_route();
		int									get_fd() const;
		std::size_t							get_int_content_length() const;
		const VirtualServ*					get_serv() const;

		void								set_redirect(const std::string& str);
		void								set_url(const std::string& str);
		void								set_route(const Route& route);
		void								set_fd(int val);
		void								set_body(const std::string& str);
		void								set_location(const std::string& str);
		void								set_status(const std::string& str);
		void								set_protocole_version(const std::string& str);
		void								set_content_length(const std::string& str);
		void								set_boundary(const std::string& str);
		void								set_content_type(const std::string& str);
		void								set_target(const std::string& str);
		void								set_auth(const std::string& str);
		void								set_upload(const std::string& str);
		void								set_serv(const VirtualServ* to_set);
		void								set_host(const std::string& str);
		void								set_error(int fd, const VirtualServ *s, const std::string& str, int code, void *data);
		void								add_env(const std::string& str, const std::string& value);
		void								add_body(std::string body);
		std::map<std::string, std::string>&	get_cgi_env();
	protected:

		int						fd;
		std::string				error_header(int code);
		std::map<std::string, std::string>			cgi_env;
		std::string								location;
		std::string								status;
		std::string								protocole_version;
		std::string								content_length;
		std::size_t								int_content_length;
		std::string								content_type;
		std::string								target;
		std::string								url;
		std::string								auth;
		std::string								upload;
		std::string								host;
		std::string								body;
		std::string								cgi_response;
		std::string								boundary;
		bool									first;
		char									buff[BUFF_SIZE];
		Error									err;
		const VirtualServ						*serv;
		Route									route;
		int										loop;
		std::string								redirect;


		//CGI
		int	cgi_header();
		void	set_var_env();
		int 	set_up_cgi(int fd);
		int		cgi_handler(int fd);
		int		method;
		int		p_write[2];
		int		p_read[2];
		bool	children;
		pid_t	pid;
};



#endif