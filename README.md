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

### Config file usage ###

```conf
server {
    server_name     <name>;
    listen           <port>;
    root             <root directory>;
    default          <default file>;
    protocol         <allowed methods>;
    host             <IP address>;

    # Location blocks for different URL paths
    location / {
        methods <allowed methods>;
        root <root directory>;
        cgi <on/off>;
        autoindex <on/off>;
        upload_path <upload directory>;
    }
    
    # Additional location blocks for different paths (e.g., /cgi/)
}
```
### How to run the project

### 1. Clone the Repository
To get started, clone the Webserv repository to your local machine:
```bash
git clone <repository_url>
cd <repository_directory>
```
### 2. Build the project

Compile using the command make

### 3. Run the server

Without arguments will take the config.cf file:
```
./server
```
With arguments for a custom .cf file as follow:
```
./server <path_to_custom_config_file>
```

