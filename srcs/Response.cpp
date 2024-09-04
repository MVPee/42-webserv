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


static std::string Receive_Content_Header(int &client_fd, Server &server, ssize_t &body_size) {
	ssize_t				bytes_read = 0;
    char				buffer = 0;
    std::ostringstream	content_stream;

	while ((bytes_read = recv(client_fd, &buffer, 1, 0)) > 0) {
		body_size += bytes_read;
		content_stream << buffer;
		if ((content_stream.str()).find("\r\n\r\n") != std::string::npos) break;
	}
	if (bytes_read == (ssize_t) -1) throw std::runtime_error("Receive failed");
	// if (bytes_read == 0) throw std::runtime_error("Connexion closed"); //? Necessary ?
	if (body_size > server.getBody()) throw std::runtime_error("Body too large");

	return (content_stream.str());
}

static void Output_Content_Body(int &client_fd, ssize_t &body_size, Server &server, std::ofstream &output_file, std::string &boundary) {
	ssize_t		 bytes_read 	= 0;
	char		buffer[1024] = {0};
	std::string	old_buffer;
	bool		is_second_iteration = false;

	//TODO: supprimer le fichier en cas d'érreur


	//TODO: S'arréter à la boundary pour plusieurs images (stocker dans un buffer de réponse)
	while((bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {

		if (is_second_iteration || bytes_read < (sizeof(buffer) - 1))
		{
			std::string full = old_buffer + std::string(buffer, bytes_read);
			std::size_t end = full.find(boundary);
			if (end != std::string::npos)
			{
				// std::cout << G << "found" << C << std::endl; //? DEBUG
				output_file.write(full.c_str(), end);
				break;
			}
			else output_file << full;
		}
		is_second_iteration = !is_second_iteration;
		body_size += bytes_read;
		old_buffer.assign(buffer, bytes_read);

	}
	if (bytes_read == (ssize_t) -1) throw std::runtime_error("Receive failed");
	// if (bytes_read == 0) throw std::runtime_error("Connexion closed"); //? Necessary ?
	if (body_size > server.getBody()) throw std::runtime_error("Body too large");
	output_file.close();
}

static void post(int &client_fd, Request &request, Server &server) {

	ssize_t body_size = 0;
    std::string content = Receive_Content_Header(client_fd, server, body_size);
	std::string header = request.getHeader();
    std::string webkitFormBoundary;
    std::string contentDisposition;
    std::string contentType;
    std::string filename;
    std::string data;

    std::istringstream contentStream(content);
    std::string line;

	getline(contentStream, webkitFormBoundary, '\r');
	// std::cout << R << "'" + webkitFormBoundary + "'" << std::endl; //? DEBUG

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
			else throw std::runtime_error("No ContentDisposition");

            //? FILENAME
            size_t filenamePos = line.find("filename=\"");
            if (filenamePos != std::string::npos) {
                filename = line.substr(filenamePos + 10);
                filename = filename.substr(0, filename.find("\""));
            }
			else throw std::runtime_error("No filename");
            //? CONTENT-TYPE
            if (std::getline(contentStream, line) && line.find("Content-Type:") != std::string::npos)
                contentType = line.substr(14);

            //? EMPTY LINE
            std::getline(contentStream, line);
			std::cout << R << "Opening: '" << server.getRoot() + "/" + filename << "'" << C << std::endl;
			std::ofstream output_file(std::string(server.getRoot() + "/" + filename).c_str(), std::ios::trunc | std::ios::binary);
			if (!output_file.is_open() || !output_file.good()) throw std::runtime_error("Could not open file: " + filename);

            //? READ DATA UNTIL BOUNDARY
			Output_Content_Body(client_fd, body_size, server, output_file, webkitFormBoundary);
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