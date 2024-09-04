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

static void post(Request &request, Server &server) {
    std::string content = request.getContent();
    std::string webkitFormBoundary;
    std::string contentDisposition;
    std::string contentType;
    std::string filename;
    std::string data;

    std::istringstream contentStream(content);
    std::string line;
    
    if (std::getline(contentStream, line)) {
        webkitFormBoundary = line;
    }

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

            //? READ DATA UNTIL BOUNDARY
            std::ostringstream dataStream;
            while (std::getline(contentStream, line)) {
                if (line.find(webkitFormBoundary) == 0) break;
                dataStream << line << "\n";
            }
            data = dataStream.str();
            data += '\0';
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
	    post(request, server);
    }
    if (request.getLocation()->getMethods(GET)) {
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
        file.open(std::string(request.getLocation()->getRoot() + '/' + request.getLocation()->getErrorPage()).c_str());
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