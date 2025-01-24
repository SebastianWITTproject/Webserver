#include "Error.hpp"
#include <map>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sstream>
// for write


	std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


std::string generateErrorPage(const std::string& title, const std::string& message) {
    return "<!doctype html>\n"
           "<html>\n"
           "<head>\n"
           "    <title>" + title + "</title>\n"
           "</head>\n"
           "<body>\n"
           "    <h1>" + title + "</h1>\n"
           "    <p>" + message + "</p>\n"
           "</body>\n"
           "</html>\n";
}

std::string e406(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("406 Not Acceptable", "The server cannot produce a response matching the request.");
}

std::string e405(const VirtualServ* serv, std::string& header, void *data) {
    std::string methods = "Allow: ";
    int* prot = (int *)data;
    if (*prot & GET) methods.append("GET ");
    if (*prot & POST) methods.append("POST ");
    if (*prot & DELETE) methods.append("DELETE ");
	header += methods;
	header += "\r\n";
	(void) serv;
    (void)header;
	return generateErrorPage("405 Method Not Allowed", "The method is not allowed.\n");
}

std::string e404(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("404 Not Found", "The requested resource was not found on this server.");
}

std::string e403(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("403 Forbidden", "Access to the requested resource is forbidden.");
}

std::string e413(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("413 Payload Too Large", "The request is larger than the server is willing or able to process.");
}

std::string e500(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("500 Internal Server Error", "The server encountered an internal error and could not complete your request.");
}

std::string e501(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("501 not Implemented", "The server only support GET POST DELETE.");
}

std::string e505(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("505 HTTP version not supported", "The server only support HTTP/1.1.");
}

std::string e411(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("411 Length required", "the content length is required.");
}

// std::string e413(const VirtualServ* serv, std::string& header, void *data) {
//     (void)serv;
//     (void)header;
// 	return generateErrorPage("413 Content too large", "The content are too large");
// }

std::string e400(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	return generateErrorPage("400 Bad Request", "The server could not understand the request due to invalid syntax.");
}

std::string e415(const VirtualServ* serv, std::string& header, void *data) {
    (void)serv;
    (void)header;
	(void)data;
	header += "Accept-Post: multipart/form-data\r\n";
	return generateErrorPage("415 Unsupported Media Type", "The server does not support the content type.");
}

std::string e301(const VirtualServ* serv, std::string& header, void *data) {
	(void)serv;
	(void)data;
	header += "location: ";
	header += *(std::string*)data;
	header += "\r\n";
	return generateErrorPage("301 Moved Permanently", "The ressources have been moved.");
}

std::map<int, Error_function> fill_function() {
    std::map<int, Error_function> ret;
	ret[301] = e301;
    ret[405] = e405;
    ret[403] = e403;
    ret[404] = e404;
    ret[406] = e406;
    ret[413] = e413;
    ret[500] = e500;
    ret[400] = e400;
	ret[501] = e501;
	ret[505] = e505;
	ret[415] = e415;
    return ret;
}

std::map<int, std::string> status_messages;
void fill_status_messages() {
    status_messages[400] = "Bad Request";
    status_messages[403] = "Forbidden";
    status_messages[404] = "Not Found";
    status_messages[405] = "Method Not Allowed";
    status_messages[406] = "Not Acceptable";
    status_messages[413] = "Payload Too Large";
	status_messages[411] = "Content Length required";
    status_messages[500] = "Internal Server Error";
	status_messages[501] = "Method not supported";
	status_messages[505] = "HTTP Version Not Supported";
	status_messages[301] = "Moved Permanently";
}




Error::Error() : fd(-1), serv(NULL), error(0), str_error(""), data(0) {}

Error::Error(int fd, const VirtualServ* serv, int error, std::string str_error)
    : fd(fd), serv(serv), error(error), str_error(str_error) {}

void Error::set_error(int fd, const VirtualServ* serv, std::string str_error, int error, void *data) {
    this->fd = fd;
    this->serv = serv;
    this->error = error;
    this->str_error = str_error;
	this->data = data;
}

std::string load_error_page(const std::string& page_path) {
    std::ifstream file(page_path.c_str());
    std::string content;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        file.close();
    } else {
        content = generateErrorPage("404 Not Found", "The requested resource was not found on this server.");
    }
    return content;
}

std::string Error::get_error(int code, const VirtualServ* serv, std::string& header, void* data) {
    std::string body;

    if (code == 404) {

        std::string custom_page = serv->get_error();
		std::cout << custom_page << "it is the error" << std::endl;
        if (custom_page != "") {

            body = load_error_page(custom_page);
        } else {

            body = generateErrorPage("404 Not Found", "The requested resource was not found on this server.");
        }
    } else {

        body = function_arr.at(code)(serv, header, data);
    }

    return body;
}


int Error::trap_card_activate() {
    if (!*this)
        throw std::exception();
    return handle_error(fd, serv, str_error, error, data);
}


int Error::handle_error(int fd, const VirtualServ* serv, std::string error_code, int error, void *data) {


	 if (status_messages.empty()) {
        fill_status_messages();
    }

    std::string reason_phrase = status_messages[error];
	std::string	header = "";

    std::string body = Error::get_error(error, serv, header, data);


    std::string response;
    response.append("HTTP/1.1 ").append(error_code).append(" ").append(reason_phrase).append("\r\n");
    response.append("Content-Type: text/html\r\n");
    response.append("Content-Length: ").append(intToString(body.size())).append("\r\n");
    response.append(header).append("\r\n");
    response.append(body);


    std::cerr << "ERROR RESPONSE:\n" << response << std::endl;
    write(fd, response.c_str(), response.size());
    return 2;
}

Error::operator bool() const {
    return fd != -1;
}


std::map<int, Error_function> Error::function_arr = fill_function();