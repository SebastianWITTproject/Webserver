#include "Post.hpp"

Post::Post() : Request(), method(0)
{

}

int	Post::response(int fd)
{
	if (loop > 0)
		loop--;
	if (int_content_length > body.size())
	{
		std::cout << "loop: " << loop << std::endl;
		if (loop <= 0)
		{
			Error::handle_error(fd, serv, "400", 400, 0);
			return (CLOSE);
		}
		return (0);
	}
	if (err)
	{
		std::cout << "FFSFSFKSMFKSFML entering post" << std::endl;
		return (err.trap_card_activate());
	}
	if (!first)
	{
		std::cout << "entering post" << std::endl;
		if (!(route.get_methods() & POST))
		{
			int data = route.get_methods();
			return (Error::handle_error(fd, serv, "405", 405, &data));
		}
		if (target.find(".cgi") != std::string::npos)
		{
			if (!route.is_cgi_enabled())
				return (Error::handle_error(fd, serv, "403", 403, 0));
			method = CGI;
			first = 1;
			return (set_up_cgi(fd));
		}
		else if (route.get_upload_path().size() != 0)
		{
			std::cout << "entering route" << std::endl;
			method = UPLOAD;
		}
		else
		{
			std::cout << "processing error" << std::endl;
			first = 1;
			return (Error::handle_error(fd, serv, "403", 403, 0));
		}
		first = 1;
	}
	else
	{
		switch(method) {
			case CGI :
				return (cgi_handler(fd));
				break;
			case UPLOAD :
				return (handle_upload(fd));
				break;
	}
	}
	return (0);
}

int Post::handle_upload(int fd)
{
	try
	{
		std::string filename = handle_multipart(fd);
		std::cout << "Filename :" << filename << std::endl;
		if (filename.size())
		{
			std::string	response;

			response += "HTTP/1.1 201 Created\r\n";
			response += "Location:";
			response += std::string("host") + route.get_upload_path() + "/" + filename;
			response.append("\r\n\r\n");
			int i = write(fd, response.c_str(), response.size());
			if ( i == -1 || i == 0)
				return (CLOSE);
			std::cout << response << "RESPONSE_END" << std::endl;
			return (1);
		}
		return (Error::handle_error(fd, get_serv(), "400", 400, 0));
	}
	catch(std::exception& e)
	{
		return (CLOSE);
	}
}

std::string Post::handle_multipart(int fd)
{
    std::cout << "[DEBUG] Starting handle_multipart function." << std::endl;
	std::string ret;
    // Step 1: Extract the Content-Type from the request
    std::string content_type = get_content_type();
    if (content_type.empty())
    {
        std::cout << "[ERROR] Content-Type is missing in the request." << std::endl;
        Error::handle_error(fd, get_serv(), "400", 400, 0);
		throw(std::exception());
    }

    std::cout << "[DEBUG] Content-Type: " << content_type << std::endl;

    // Step 2: Ensure the Content-Type is multipart/form-data
    if (content_type.find("multipart/form-data") == std::string::npos)
    {
        std::cout << "[ERROR] Content-Type is not multipart/form-data." << std::endl;
        Error::handle_error(fd, get_serv(), "415", 415, 0);
		throw(std::exception());
    }

    // Step 3: Extract the boundary from the request
    std::string boundary = get_boundary();
    if (boundary.empty())
    {
        std::cout << "[ERROR] Boundary value is missing." << std::endl;
        Error::handle_error(fd, get_serv(), "400", 400, 0);
		throw(std::exception());
    }

    boundary = "--" + boundary; // Prepend boundary marker

    std::cout << "[DEBUG] Extracted boundary: " << boundary << std::endl;

    // Step 4: Extract the body of the request
     std::string bodi = get_body();
    if (bodi.empty())
    {
        std::cout << "[ERROR] Request bodi is empty." << std::endl;
        Error::handle_error(fd, get_serv(), "400", 400, 0);
		throw(std::exception());
    }

    std::cout << "[DEBUG] Request bodi size: " << bodi.size() << std::endl;

    // Step 5: Parse the bodi using the boundary
	std::string::size_type pos = bodi.find(boundary);
    while (pos != std::string::npos)
    {
		pos = bodi.find(boundary);
		std::cout << "[DEBUG] Bodi: " << bodi<< std::endl;
        std::string::size_type end_pos = bodi.find(boundary, pos + boundary.length());
        if (end_pos == std::string::npos)
        {
            std::cout << "[ERROR] Boundary end not found." << std::endl;
            break;
        }

        std::string part = bodi.substr(pos + boundary.length(), end_pos - pos - boundary.length());
        pos = end_pos;

        std::cout << "[DEBUG] Processing part: " << part.substr(0, 50) << "..." << std::endl;

       // Step 6: Parse each part for headers and content
std::cout << "[DEBUG] Current part: " << part.substr(0, 100) << "..." << std::endl;

std::string::size_type header_end = part.find("\r\n\r\n");
if (header_end == std::string::npos)
{
    std::cout << "[ERROR] Part headers missing or header delimiter not found." << std::endl;
    continue;
}

std::string headers_part = part.substr(0, header_end);
std::string content_part = part.substr(header_end + 4); // Skip the \r\n\r\n

headers_part.erase(0, headers_part.find_first_not_of(" \t\r\n"));
std::cout << "[DEBUG] Extracted headers part: " << headers_part << std::endl;
std::cout << "[DEBUG] Extracted content part size: " << content_part.size() << std::endl;


// Parse headers
std::istringstream headers_stream(headers_part);
std::string header_line;
std::string disposition;

while (std::getline(headers_stream, header_line))
{

    std::cout << "[DEBUG] Parsed header line: " << header_line << std::endl;

    if (header_line.empty())
        break;

    if (header_line.find("Content-Disposition:") != std::string::npos)
    {
        disposition = header_line;
        std::cout << "[DEBUG] Found Content-Disposition: " << disposition << std::endl;
    }
}

// Validate Content-Disposition
if (disposition.empty() || disposition.find("form-data") == std::string::npos)
{
    std::cout << "[ERROR] Invalid or missing Content-Disposition header." << std::endl;
    continue;
}

        // Extract the filename (if present)
        std::string::size_type filename_pos = disposition.find("filename=");
        if (filename_pos != std::string::npos)
        {
            filename_pos += 9; // Move past "filename="
            std::string::size_type filename_end = disposition.find("\"", filename_pos + 1);
            std::string filename = disposition.substr(filename_pos + 1, filename_end - filename_pos - 1);

            std::cout << "[DEBUG] Extracted filename: " << filename << std::endl;

            // Step 7: Save the file to the destination defined in the config
            std::string upload_path = route.get_upload_path();
            if (upload_path.empty())
            {
                std::cout << "[ERROR] Upload path is not set." << std::endl;
                Error::handle_error(fd, get_serv(), "500", 500, 0);
				throw(std::exception());
            }
			ret = filename;
            std::ofstream outfile((route.get_root() + upload_path + "/" + filename).c_str(), std::ios::binary);
            if (!outfile)
            {
                std::cout << "[ERROR] Failed to open file for writing: " << upload_path + "/" + filename << std::endl;
                Error::handle_error(fd, get_serv(), "500", 500, 0);
				throw(std::exception());
            }

            outfile.write(content_part.c_str(), content_part.size());
            outfile.close();
			bodi = bodi.substr(pos);
            std::cout << "[DEBUG] File saved successfully: " << filename << std::endl;
        }
    }
    std::cout << "[DEBUG] handle_multipart function completed successfully." << std::endl;
    return ret; // Success
}



std::string	Post::type()
{
	return ("POST");
}

int	Post::type_code()
{
	return (POST);
}

Post::~Post()
{

}