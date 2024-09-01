#include "../includes/Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

//? Temporary function
std::string get_content_type(std::string extension)
{
    std::map<std::string, std::string> extensions;
    extensions["html"] = "text/html";
    extensions["php"] = "text/php";
    extensions["ico"] = "image/x-icon";
    extensions["png"] = "image/png";
    extensions["jpg"] = "image/jpg";
    extensions["None"] = "text/html";

	if (extensions.count(extension)) return (extensions[extension]);
	else return "text/html";
}

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

static void post(int &client_fd, Request &request, Server &server) {
	//? Retrieve Body of request aka Content
    char buffer;
    std::ostringstream content_stream;
    int fd = 0;

	while ((fd = recv(client_fd, &buffer, 1, 0)) != 0)
	{
		if (fd < 0) throw std::runtime_error("Receive failed");
		content_stream << buffer;
		if ((content_stream.str()).find("\r\n\r\n") != std::string::npos) break;
	}

    std::string content = content_stream.str();
	std::string header = request.getHeader();
    std::string webkitFormBoundary = header.substr(header.find("boundary=") + 9, header.find('\n', header.find("boundary=")) - header.find("boundary="));
    std::string contentDisposition;
    std::string contentType;
    std::string filename;
    std::string data;

    std::istringstream contentStream(content);
    std::string line;

	std::cout << M << content << C << std::endl;

    while (std::getline(contentStream, line)) {
        if (line.find("Content-Disposition:") != std::string::npos) {
            contentDisposition = line;

            //? CONTENT-DISPOSITION
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string dispositionType = line.substr(pos + 2);
                size_t semicolonPos = dispositionType.find(";");
                if (semicolonPos != std::string::npos)
                    dispositionType = dispositionType.substr(0, semicolonPos);
                contentDisposition = dispositionType;
            }

            //? FILENAME
            size_t filenamePos = line.find("filename=\"");
            if (filenamePos != std::string::npos) {
                filename = line.substr(filenamePos + 10);
                filename = filename.substr(0, filename.find("\""));
            }

            //? CONTENT-TYPE
            if (std::getline(contentStream, line) && line.find("Content-Type:") != std::string::npos)
                contentType = line.substr(14);

            //? EMPTY LINE
            std::getline(contentStream, line);

			std::ofstream output_file(std::string(server.getRoot() + "/" + filename).c_str(), std::ios::trunc | std::ios::binary);
				if (!output_file.is_open() || !output_file.good()) throw std::runtime_error("Could not open file: " + filename);

            //? READ DATA UNTIL BOUNDARY
			char buffer2[1024];
			std::size_t i = 0;
			while(i < request.getContent_Length() - content.size())
			{
				fd = recv(client_fd, &buffer2, sizeof(buffer2) - 1, 0);
				if (fd < 0) throw std::runtime_error("Receive failed");
				else if (fd == 0) throw std::runtime_error("connexion closed");
				buffer2[fd] = 0;

				for (std::size_t j = 0; j < fd && j + i < request.getContent_Length() - content.size(); j++)
					output_file << buffer2[j];
				i+=fd;
			}
			output_file.close();
        }
    }
    std::cout << B << "Boundary: " << webkitFormBoundary << "\n";
    std::cout << "Content-Disposition: " << contentDisposition << "\n";
    std::cout << "Filename: " << filename << "\n";
    std::cout << "Content-Type: " << contentType << "\n";
    std::cout << "Data: " << data << "\n" << C << std::endl;
}

Response::Response(int &client_fd, Request &request, Server &server) {
    
    if (request.getMethod() == POST) {
        std::cout << R "POST" C << std::endl;
	    post(client_fd, request, server);
    }
    if (server.getMethods(GET)) {
	    getContent(request, server);
        _fd = send(client_fd, _content.c_str(), _content_size, 0);
        if (_fd < 0) {
            throw std::runtime_error("Send failed");
        }
    }
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {
    if (_fd >= 0)
	    close(_fd);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Response const & i ) {
	(void)i;
	//o << "Value = " << i.getValue();
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

static void putContent(const std::ifstream &file, std::string &_content) {
    std::ostringstream temp;

    temp << file.rdbuf();
    std::string content = temp.str();
    std::ostringstream sizeStream;
    sizeStream << content.size();
    _content += sizeStream.str() + "\n\n" + content;
}

void Response::getContent(Request &request, Server &server) {
    std::ifstream file(request.getPath().c_str(), std::ios::binary);

    _content += "HTTP/1.1 ";
    if (!file.is_open() || !file.good()) {
        file.open(std::string(server.getRoot() + '/' + server.getError()).c_str());
        _content += "404 NOT FOUND\nContent-Type: text/html\nContent-Length: ";
        if (!file.is_open() || !file.good())
            _content += "21\n\n<h1>default: 404</h1>";
        else
            putContent(file, _content);
    }
    else {
        _content += "200 OK\nContent-Type: " + get_content_type(request.getExtension()) + "\nContent-Length: ";
        putContent(file, _content);
    }
    if (file.is_open())
        file.close();
	_content_size = _content.size();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */