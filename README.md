### Overview ###

Webserv is a HTTP server written in C++. It handles multiple client connections efficiently using poll() and supports essential web server features like static file hosting, CGI execution, and HTTP request handling.
This server handles the HTTP/1.1 protocol, managing requests, responses, and connections in accordance with the protocol.
This application features a configuration file inspired by NGINX, allowing users to define server settings, routes, and CGI execution rules.

### Features ###

- ✅ **HTTP/1.1 Protocol Support**
- ✅ **Multiple connection handling with `poll()`** for multiplexing
- ✅ **Handles GET, POST, DELETE methods**
- ✅ **Static File Hosting**
- ✅ **Supports file upload with option for multiple upload at the same time**
- ✅ **CGI Execution**
- ✅ **Config file for server settings, custom routes**
- ✅ **Custom Error Pages**
- ✅ **Directory Listing**

