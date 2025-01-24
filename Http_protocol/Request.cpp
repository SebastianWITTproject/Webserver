#include "Request.hpp"

std::map<std::string, Parsed_header> make_header_func();


const char *Test::what() const throw()
{
	return ("test");
}
//verifier tous les fields
Request::Request()
	: location(""),
	status(""),
	protocole_version(""),
	content_length(""),
	int_content_length(0),
	content_type(""),
	target(""),
	url(""),
	auth(""),
	upload(""),
	host(""),
	body(""),
	cgi_response(""),
	boundary(""),
	first(0),
	err(),
	serv(0),
	route(),
	loop(3000),
	redirect("")
{
	for (int i = 0; i < 10000; i++)
		buff[i] = 0;
}

Request::~Request()
{

}

void	Request::set_auth(const std::string& str)
{
	auth = str;
}

void	Request::set_content_length(const std::string& str)
{
	content_length = str;
	int_content_length = std::atoi(str.c_str());
}

void	Request::set_content_type(const std::string& str)
{
	content_type = str;
}

void	Request::set_protocole_version(const std::string& str)
{
	protocole_version = str;
}

void	Request::set_host(const std::string& str)
{
	host = str;
}

void	Request::set_status(const std::string& str)
{
	status = str;
}

void	Request::set_url(const std::string& str)
{
	url = str;
}

void	Request::set_location(const std::string& str)
{
	location = str;
}

void	Request::set_serv(const VirtualServ* to_set)
{
	serv = to_set;
}

void	Request::set_target(const std::string& str)
{
	target = str;
}

void	Request::set_body(const std::string& str)
{
	body = str;
}

void	Request::set_boundary(const std::string& str)
{
	boundary = str;
}

void	Request::set_upload(const std::string& str)
{
	upload = str;
}

void	Request::set_redirect(const std::string& str)
{
	redirect = str;
}

void	Request::set_error(int fd, const VirtualServ *s, const std::string& str, int code, void *data)
{
	err.set_error(fd, s, str, code, data);
}

void	Request::add_env(const std::string& str, const std::string& value)
{
	cgi_env[str] = value;
}

void	Request::set_fd(int val)
{
	fd = val;
}

void	Request::set_route(const Route& str)
{
	route =str;
}


Error&	Request::get_error()
{
	return (err);
}
std::string	Request::get_location() const
{
	return (location);
}

std::string	Request::get_status() const
{
	return (status);
}

std::string	Request::get_content_length() const
{
	return (content_length);
}

const std::string&	Request::get_body() const
{
	return (body);
}

std::string	Request::get_url() const
{
	return (url);
}

std::string& Request::get_redirec()
{
	return (redirect);
}

std::string	Request::get_content_type() const
{
	return (content_type);
}

std::string	Request::get_protocole_version() const
{
	return (protocole_version);
}

std::string Request::get_host() const
{
	return (host);
}

std::string	Request::get_boundary() const
{
	return (boundary);
}

std::string	Request::get_target() const
{
	return (target);
}

std::string	Request::get_upload() const
{
	return (upload);
}

std::string	Request::get_auth() const
{
	return (auth);
}

Route&	Request::get_route()
{
	return (route);
}

std::size_t	Request::get_int_content_length() const
{
	return (int_content_length);
}

int	Request::get_fd() const
{
	return (fd);
}

const VirtualServ*	Request::get_serv() const
{
	return (serv);
}

std::map<std::string, std::string>& Request::get_cgi_env()
{
	return (cgi_env);
}

//CGIIIIIIII

static void	set_up_child(int *read, int *write)
{
	if (pipe(read) == -1)
		throw(std::exception());
	if (pipe(write) == -1)
	{
		close(read[0]);
		close(read[1]);
		throw(std::exception());
	}
}

static void stcpy(char *a, const char *b, int size)
{
	int i = 0;

	while (i < size && b[i])
	{
		a[i] = b[i];
		i++;
	}
}

static void	set_my_env(char **envp, std::map<std::string, std::string>& env)
{
	std::map<std::string, std::string>::iterator it = env.begin();
	envp = new char*[20];
	for (int i = 0; i < 20; i++)
		envp[i] = new char[200];
	for (int i = 0; i < 20 && it != env.end(); i++,it++)
		stcpy(envp[i], (it->first + it->second).c_str(), 200);
}

void	Request::set_var_env()
{
		cgi_env["SERVER_PROTOCOLE="] = PROTOCOLE;
		cgi_env["GATEWAY_INTERFACE="] = GATEWAY;
		cgi_env["REMOTE_HOST="] = "";
		cgi_env["REMOTE_ADDR="] = "";
		cgi_env["REQUEST_METHOD="] = this->type();
		cgi_env["SERVER_NAME="] = serv->get_name();
		cgi_env["SCRIPT_NAME="] = target;
		cgi_env["CONTENT_TYPE="] = content_type;
		cgi_env["CONTENT_LENGTH="] = content_length;
		cgi_env["SERVER_PORT="] = serv->get_port();
		cgi_env["SERVER_NAME="] = serv->get_name();
		cgi_env["PATH_INFO="] = target;
}

int Request::set_up_cgi(int fd)
{
	std::cout << "TARGET CGI: " << target << std::endl;
	set_up_child(&p_read[0], &p_write[0]);
	set_var_env();
	if (access(target.c_str(), F_OK | X_OK) != 0)
		return (Error::handle_error(fd, serv, "404", 404, 0));
	if (fcntl(p_write[1], F_SETFL, O_NONBLOCK) < 0)
			throw(std::exception());
	if (int_content_length > body.size())
		write(p_write[1], body.c_str(), body.size());
	else
		write(p_write[1], body.c_str(), int_content_length);
	pid = fork();

	if (pid == -1)
		return (Error::handle_error(fd, serv, "500", 500, 0));
	if (!pid)
	{
		char **envp = 0;
		if (dup2(p_write[0], 0) == -1)
			std::exit(0);
		if (dup2(p_read[1], 1) == -1)
			std::exit(0);
		set_my_env(envp, cgi_env);
		if (execve(target.c_str(), envp, envp) == -1)
			std::exit(0);
	}
	else
	{
		children = 1;
		close(p_write[0]);
		if (fcntl(p_read[0], F_SETFL, O_NONBLOCK) < 0)
			throw(std::exception());
		close(p_write[1]);
	}
	return (0);
}

static std::string	next_word(std::string& sub, std::size_t& i) // le mettre dans req_function
{
	std::string	ret = sub.substr(i);
	std::size_t start = ret.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		throw std::exception();
	ret = ret.substr(start);
	std::size_t	pos = ret.find_first_of(" \t\r\n");

	if (pos == std::string::npos)
	{
		ret = ret.substr(0);
		throw std::exception();
	}
	else
	{
		ret = ret.substr(0, pos);
		i += pos + start;
	}
	return (ret);
}

int	Request::cgi_header()
{
	std::string	header = buff;
	std::size_t	pos = header.find("Status:");
	std::size_t	temp = pos;
	std::string code = "200";

	if (pos != std::string::npos)
	{
		pos += 7;
		code = next_word(header, pos);
		cgi_response.append("HTTP/1.1 ").append(code).append("\n");
		cgi_response.append(header.substr(0, temp)).append(header.substr(pos));
		std::cout << "Size1 : " << cgi_response.size() << std::endl;
		int i = write(fd, cgi_response.c_str(), cgi_response.size());
		if (i == 0 || i == -1)
			return (CLOSE);
	}
	else
	{
		cgi_response.append("HTTP/1.1 200 OK\n");
		cgi_response.append(header);
		std::cout << "Size2 : " << cgi_response.size() << std::endl;
		int i = write(fd, cgi_response.c_str(), cgi_response.size());
		if (i == 0 || i == -1)
			return (CLOSE);
	}
	return (0);
}

int	Request::cgi_handler(int fd)
{
	int	status;

	loop--;
	if (waitpid(pid, &status, WNOHANG) > 0)
		children = 0;
	int size = read(p_read[0], &buff[0], 8196);//mettre le read en non bloquant sinon les problemes;
	if (loop == 0)
	{
		kill(pid, SIGINT);
		Error::handle_error(fd, serv, "500", 500, 0);
		if (waitpid(pid, &status, WNOHANG) > 0)
			children = 0;
		return (1);
	}
	if (size == 0 || size == -1)
	{
		if (children == 1)
			return (0);
		close(p_read[0]);
		close(p_read[1]);
		return (CLOSE);
	}
	if (cgi_response.size() == 0)
		return (cgi_header());
	else
	{
		int i = write(fd, buff, size);
		if ( i == -1 || i == 0)
			return (CLOSE);
	}

	return (0);
}