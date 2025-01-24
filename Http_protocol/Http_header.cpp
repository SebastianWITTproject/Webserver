#include "Http_header.hpp"

std::map<std::string, Parsed_header> make_header_func()
{
	std::map<std::string, Parsed_header>	ret;

	ret["Content-Type:"] = Http_header::content_type;
	ret["Content-Length:"] = Http_header::content_length;
	ret["Location:"] = Http_header::location;
	ret["Status:"] = Http_header::status;
	ret["Authorization"] = Http_header::authorization;
	ret["Host:"] = Http_header::host;
	return (ret);
}

std::map<std::string, Parsed_header> Http_header::header_func = make_header_func();

static std::string	next_word(std::string& sub, std::size_t& i, const char* delim)
{
	std::string	ret = sub.substr(i);
	std::size_t start = ret.find_first_not_of(delim);
	if (start == std::string::npos)
		throw std::exception();
	ret = ret.substr(start);
	std::size_t	pos = ret.find_first_of(delim);

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
	std::cerr << ret << std::endl;
	return (ret);
}

void	Http_header::location(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv)
{
	(void)serv;
	std::string	temp = toRead.substr(pos);
	std::size_t	end = temp.find("\n");

	if (end == std::string::npos)
		throw(std::exception());
	req->set_location(next_word(toRead, pos, " \t\r\n"));
}

void	Http_header::content_length(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv)
{
	(void)serv;
	std::string	temp = toRead.substr(pos);
	std::size_t	end = temp.find("\n");

	if (end == std::string::npos)
		throw(std::exception());
	req->set_content_length(next_word(toRead, pos, " \t\r\n"));
}

void	Http_header::content_type(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv)
{
	(void)serv;
	std::string	temp = toRead.substr(pos);
	std::size_t	end = temp.find("\n");

	if (end == std::string::npos)
		throw(std::exception());
	req->set_content_type(next_word(toRead, pos, " \t\r\n;"));
	std::cout << "content type = " << req->get_content_type() << std::endl;
	if (req->get_content_type() == "multipart/form-data")
	{
		next_word(toRead, pos, " \r\n\t=;");
		req->set_boundary(next_word(toRead, pos, "\" \r\n\t="));
		std::cout << "boundary :" << req->get_boundary() << std::endl;
	}
}

void	Http_header::status(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv)
{
	(void)serv;
	std::string	temp = toRead.substr(pos);
	std::size_t	end = temp.find("\n");

	if (end == std::string::npos)
		throw(std::exception());
	req->set_status(next_word(toRead, pos, " \t\r\n"));
}

void	Http_header::host(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv)
{
	(void)serv;
	std::string	temp = toRead.substr(pos);
	std::size_t	end = temp.find("\n");

	if (end == std::string::npos)
		throw(std::exception());
	if (req->get_serv() != 0)
		req->set_error(req->get_fd(), req->get_serv(), "400", 400, 0);
	std::string host = next_word(toRead, pos, " \t\r\n:");
	std::string port = next_word(toRead, pos, " \t\r\n:");
	req->set_host(host + ":" + port);
	std::cout << "THE HOST : " << req->get_host() <<std::endl;
	req->set_serv(serv->find(host, port ));
}

void	Http_header::authorization(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv)
{
	(void)serv;
	std::string	temp = toRead.substr(pos);
	std::size_t	end = temp.find("\n");
	std::size_t	new_line =  temp.find("\n ");
	std::size_t	final_pos = 0;


	if (end == std::string::npos)
		throw(std::exception());
	while (new_line == end && new_line != std::string::npos)
	{
		final_pos += end;
		temp = temp.substr(end);
		new_line = temp.find("\n ");
		end = temp.find("\n");
	}
	if (final_pos == 0)
		final_pos = end;
	req->set_auth(toRead.substr(pos, final_pos));
}