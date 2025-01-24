#include "Route.hpp"
std::map<std::string, ParseFunction> Route::route_directives = Route::init_route_directives();

Route::Route()
    : location(""),
      root(""),
      redirection(""),
      default_page(""),
      autoindex(false),
      allowed_methods(0),
      cgi_enabled(false),
      upload_path("")
{}

Route::~Route() {}

void Route::set_location(const std::string& loc) { location = loc; }
void Route::set_root(const std::string& root) { this->root = root; }
void Route::set_default(const std::string& default_page) { this->default_page = default_page; }
void Route::set_autoindex(bool index) { autoindex = index; }
void Route::set_methods(int methods) { allowed_methods |= methods; }

void Route::set_cgi(bool enabled) {
    cgi_enabled = enabled;
}

void Route::set_upload_path(const std::string& path) {
    upload_path = path;
}

void Route::set_redirection(const std::string& location) {
    redirection = location;
}

bool Route::is_cgi_enabled() const {
    return cgi_enabled;
}

const std::string& Route::get_upload_path() const {
    return upload_path;
}

std::string Route::get_location() const { return location; }
std::string Route::get_root() const { return root; }
std::string& Route::get_redirection() {return redirection;}
const std::string& Route::get_default() const { return default_page; }
bool Route::is_autoindex() const { return autoindex; }
int Route::get_methods() const { return allowed_methods; }

std::map<std::string, ParseFunction> Route::init_route_directives() {
    std::map<std::string, ParseFunction> ret;

    ret["location"] = Route::location_var;
    ret["root"] = Route::root_var;
    ret["autoindex"] = Route::autoindex_var;
    ret["methods"] = Route::methods_var;
	ret["default"] = Route::default_var;
	ret["cgi"] = Route::cgi_var;
    ret["upload_path"] = Route::upload_path_var;
    ret["redirection"] = Route::redirection_var;
    return ret;
}

void Route::location_var(const std::string& line, Route& route) {
    route.set_location(get_value(line, alnum_path));
}

void Route::redirection_var(const std::string& line, Route& route) {
    std::string temp = get_value(line, myascci);
    route.set_redirection(temp);
}

void Route::root_var(const std::string& line, Route& route) {
  	std::string	temp = get_value(line, alnum_path);
	check_directory_path(temp);
	route.set_root(temp);
	}


void Route::default_var(const std::string& line, Route& route) {

		std::string	temp = get_value(line, myascci);
		route.set_default(temp);

}

void Route::autoindex_var(const std::string& line, Route& route) {
    std::string temp = get_value(line, isalnum);
    route.set_autoindex(temp == "on");
}

void Route::methods_var(const std::string& line, Route& route) {
    std::string temp = get_value(line, myascci);
    if (temp.find("GET") != std::string::npos)
		route.allowed_methods |= GET;
	if (temp.find("POST") != std::string::npos)
		route.allowed_methods  |= POST;
	if (temp.find("DELETE") != std::string::npos)
		route.allowed_methods  |= DELETE;
}


void Route::cgi_var(const std::string& line, Route& route) {
    std::string temp = get_value(line, myascci);
    route.set_cgi(temp == "on");
}

void Route::upload_path_var(const std::string& line, Route& route) {
    std::string temp = get_value(line, alnum_path);
    route.set_upload_path(temp);
}