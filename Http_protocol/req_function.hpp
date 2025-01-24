#ifndef REQ_FUNCTION_HPP
#define REQ_FUNCTION_HPP
#include "../Http_protocol/Get.hpp"
#include "../Http_protocol/Post.hpp"
#include "../Http_protocol/Delete.hpp"
#define MAX_BODY 1000000
Request* 		checkRequest(const std::string& temp);
Request*		parsedRequest(int fd, ServerBlock *serv);
int				parsed_header(std::string& to_parsed, std::size_t& pos, Request* ret, ServerBlock *serv, int fd);
std::string		parse_response(char *buff, Request* req);
void			parsed_body(std::string& to_parsed, std::size_t& pos, Request* ret, ServerBlock *serv, int fd);
void			add_to_body(int fd, Request* req);
#endif