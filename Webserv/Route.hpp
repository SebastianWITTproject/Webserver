#ifndef ROUTE_HPP
#define ROUTE_HPP
#include <string>
#include <map>
#include "lib_function.hpp"

class Route;
typedef void (*ParseFunction)(const std::string& line, Route& route);

class Route {
public:
    Route();
    ~Route();

    void set_location(const std::string& loc);
    void set_root(const std::string& root);
	void set_default(const std::string& default_page);
    void set_redirection(const std::string& location);
    void set_autoindex(bool index);
    void set_methods(int methods);
	void set_cgi(bool enabled);
    void set_upload_path(const std::string& path);

	 Route(const Route& other) {
        this->location = other.location;
        this->root = other.root;
        this->allowed_methods = other.allowed_methods;
        this->redirection = other.redirection;
        this->autoindex = other.autoindex;
		this->cgi_enabled = other.cgi_enabled;
		this->default_page = other.default_page;
        this->upload_path = other.upload_path;
    }

    Route& operator=(const Route& other) {
        if (this != &other) {
            this->location = other.location;
            this->root = other.root;
            this->allowed_methods = other.allowed_methods;
            this->redirection = other.redirection;
            this->autoindex = other.autoindex;
			this->cgi_enabled = other.cgi_enabled;
			this->default_page = other.default_page;
            this->upload_path = other.upload_path;
        }
        return *this;
    }

    std::string get_location() const;
    std::string get_root() const;
	const std::string& get_default() const;
    std::string& get_redirection();
    bool is_autoindex() const;
    int get_methods() const;
	bool is_cgi_enabled() const;
    const std::string& get_upload_path() const;

    static void location_var(const std::string& line, Route& route);
    static void root_var(const std::string& line, Route& route);
    static void autoindex_var(const std::string& line, Route& route);
	static void default_var(const std::string& line, Route& route);
    static void methods_var(const std::string& line, Route& route);
    static void redirection_var(const std::string& line, Route& route);
	static void cgi_var(const std::string& line, Route& route);
    static void upload_path_var(const std::string& line, Route& route);
    static std::map<std::string, ParseFunction> init_route_directives();
    static std::map<std::string, ParseFunction> route_directives;


private:
    std::string location;
    std::string root;
    std::string redirection;
	std::string default_page;
    bool autoindex;
    int allowed_methods;
	bool cgi_enabled;
    std::string upload_path;

};
#endif