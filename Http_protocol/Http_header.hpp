#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP
#include "Request.hpp"
#include "../Webserv/ServerBlock.hpp"

class Request;
typedef void (*Parsed_header)(Request*, std::string& toRead, std::size_t& pos, ServerBlock *serv);

class Http_header
{
	public :

		static void	location(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static void	content_length(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static void	content_type(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static void	status(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static void	authorization(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static void	host(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static void	transfer_encoding(Request *req, std::string& toRead, std::size_t& pos, ServerBlock *serv);
		static std::map<std::string, Parsed_header>	header_func;
	private :
		Http_header();

};
#endif
