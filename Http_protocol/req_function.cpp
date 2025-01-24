#include "req_function.hpp"

Request* checkRequest(const std::string& temp)
{
	if (temp == "GET")
		return (new Get());
	else if (temp == "POST")
		return (new Post());
	else if (temp == "DELETE")
		return (new Delete());
	return (0);
}

static std::string	next_word(std::string& sub, std::size_t& i)
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

int	parsed_body(int fd, std::string& to_parsed, std::size_t& pos, Request* ret)
{
	if (ret->get_route().get_methods() & ret->type_code())
	{
		int data = ret->get_route().get_methods();
		ret->set_error(fd, ret->get_serv(), "405", 405, &data);
		return (1);
	}
	/*
	if (ret->get_content_type() == "multipart/form-data")
	{
		if (ret->type_code() != POST || ret->get_upload().size() == 0)
		{
			ret->set_error(fd, ret->get_serv(), "400", 400);
			return (1);
		}
		//inserer parsing multipart form data


	} */
	else if (ret->get_content_type().size() && ret->get_int_content_length() <= 0)
	{
		ret->set_error(fd, ret->get_serv(), "411", 411, 0);//mettre l'erreur specifique content length
		return (1);
	}
	else if (ret->get_int_content_length() > MAX_BODY || (ret->get_serv()->get_size() && ret->get_int_content_length() > ret->get_serv()->get_size()))
	{
		ret->set_error(fd, ret->get_serv(), "413", 413, 0);//mettre l'erreur specifique content length
		return (1);
	}
	else if (to_parsed.size() < ret->get_int_content_length() + pos)
		ret->set_body(to_parsed.substr(pos));
	else
		ret->set_body(to_parsed.substr(pos, ret->get_int_content_length()));
	return (0);
}

int	parsed_header(std::string& to_parsed, std::size_t& pos, Request* ret, ServerBlock *serv, int fd)
{
	(void)fd;
	try
	{
		std::map<std::string, Parsed_header>::iterator	it;
		std::string										test;
		std::size_t										temp;
		std::size_t										end = to_parsed.find("\r\n\r\n");

		if (end == std::string::npos)
			return (1);
		while (1)
		{
			if (end <= pos + 1)
				return (0);
			test = next_word(to_parsed, pos);
			it = Http_header::header_func.find(test);
			if (it == Http_header::header_func.end())
			{
				temp = to_parsed.substr(pos).find_first_of("\n");
				if (temp == std::string::npos)
					return (1);
				pos += temp;
				continue;
			}
			it->second(ret, to_parsed, pos, serv);
		}
	}
	catch(const std::exception& e)
	{
		return (1);
	}
}

//a refaire
static void	resolve_path(Request *req, std::string path, const VirtualServ *serv,
							const std::map<std::string, Route>& arr)
{
	std::cout << "PATH: " << path <<std::endl;
	std::map<std::string, Route>::const_iterator it = arr.find(path.c_str());
	if (it != arr.end())
	{
		req->set_route(it->second);
		std::cerr << "default : " << req->get_route().get_default() << std::endl;
	}
	else
	{
		std::size_t i = path.find_last_of('/');
		if (i == std::string::npos || (i == 0 && path.size() == 1))
			req->set_route(serv->get_default_route());
		else if (i == path.size() - 1)
		{
			path.erase(i);
			i = path.find_last_of('/');
			if (i == std::string::npos)
				req->set_route(serv->get_default_route());
			else
				resolve_path(req, path.substr(0, i + 1), serv, arr);
		}
		else
			resolve_path(req, path.substr(0, i + 1), serv, arr);
	}
}

Request* parsedRequest(int fd, ServerBlock *serv)
{
	Request*		ret;
	char			line[20000] = {0};
	std::string		to_parsed;
	std::size_t		pos = 0;
	std::size_t		end = 0;
	ret = 0;
	try
	{
		int i = read(fd, &line[0], 20000);
		std::cout << "READ size:" << i << std::endl;
		if (i == -1 || i == 0)
			throw(std::exception());
		to_parsed = std::string(line, i);
		end = to_parsed.find("\n");
		std::cerr << "request : " << to_parsed << std::endl;
		ret = checkRequest(next_word(to_parsed, pos));
		if (ret == 0)
		{
			ret = new Get();
			ret->set_error(fd, 0, "501", 501, 0);
			return (ret);
		}
		if (end == std::string::npos)
		{
			ret->set_error(fd, 0, "400", 400, 0);
			return (ret);
		}
		ret->set_fd(fd);
		ret->set_target(next_word(to_parsed, pos));
		ret->set_url(ret->get_target());
		if (ret->get_target().find("?") != std::string::npos)
		{
			ret->add_env("QUERY_STRING=", ret->get_target().substr(ret->get_target().find("?")));// decale de 1 pour ne pas inclure ?
			ret->set_target(ret->get_target().substr(0, ret->get_target().find("?")));
		}
		ret->set_protocole_version(next_word(to_parsed, pos));
		if (pos > end)
		{
			ret->set_error(fd, serv->get_default(), "400", 400, 0);
			return (ret);
		}
		if (ret->get_protocole_version() != "HTTP/1.1")
		{
			ret->set_error(fd, serv->get_default(), "505", 505, 0);
			return (ret);
		}
		std::cerr << "protocole:" << ret->get_protocole_version() << std::endl;
		if (parsed_header(to_parsed, pos, ret, serv, fd))
			return (ret);
		if (ret->get_serv() == 0)
		{
			ret->set_error(fd, 0, "400", 400, 0);
			return (ret);
		}
		if (parsed_body(fd, to_parsed, pos, ret))
			return (ret);
		if (ret->get_serv()->get_routes().size() == 0)
			ret->set_route(ret->get_serv()->get_default_route());
		else
			resolve_path(ret, ret->get_target(), ret->get_serv(), ret->get_serv()->get_routes());
		if (ret->get_route().get_redirection().size())
		{

			ret->set_error(fd, 0, "301", 301, &(ret->get_route().get_redirection()));
			return (ret);
		}
		ret->set_target(ret->get_target()
			.replace(0, ret->get_route().get_location().size(), ret->get_route().get_root().append("/")));
		std::cerr << "TARGERTTTTTT" << ret->get_target() << std::endl;
			// parsed_body(to_parsed, pos, ret, serv, fd);
	}// probleme de route a regler
	catch(const std::exception& e)
	{
		delete ret;
		throw(e);
	}
	return (ret);
}

void	add_to_body(int fd, Request* req)
{
	char			line[20000] = {0};
	std::string		to_parsed;
	int				i = 0;

	if (req->get_error())
	{
		req->get_error().trap_card_activate();
		throw(std::exception());
	}
	if (req->get_int_content_length() == 0)
		throw (std::exception());
	if (req->get_int_content_length() < 20000)
		i = read(fd, &line[0], req->get_int_content_length());
	else
		i = read(fd, &line[0], 20000);
	if (i == -1 || i == 0)
			throw(std::exception());
	to_parsed = std::string(line, i);
	std::cerr << "body : " << to_parsed << std::endl;
	req->set_body(req->get_body() + to_parsed);
	if (req->get_body().size() > 20000000)
	{
		Error::handle_error(fd, req->get_serv(), "413", 413, 0);
		throw(std::exception());
	}
}
