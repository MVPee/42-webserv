#include "../includes/Post.hpp"


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Post::Post(const int client_fd, Request &request, Server &server) : _client_fd(client_fd), _request(request), _server(server), _body_size(0)
{
	std::string header = request.getHeader();
	_boundary = header.substr(header.find("boundary=") + 9).c_str();
	_boundary = _boundary.substr(0, _boundary.find('\r'));
	bool flag = true;

	if (_boundary.empty())
		throw std::runtime_error("boundary not found in header");

	while (flag)
	{
		// std::cout << G << "HEYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" << C << std::endl;
		handle_post_request();
		// std::cout << R "'" << _remaining_content << "'" C << std::endl;
		if (_remaining_content.empty() || _remaining_content == _boundary + "--") flag = false;
	}
	

}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Post::~Post()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Post const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void Post::handle_post_request( void )
{
    std::string contentDisposition;
    std::string contentType;
    std::string filename;
    std::istringstream contentStream(receive_content_header());
    std::string line;

	std::cout << R  << "Header: " << contentStream.str() << C << std::endl;
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
			std::ofstream output_file(std::string(_server.getRoot() + "/" + filename).c_str(), std::ios::trunc | std::ios::binary);
			if (!output_file.is_open() || !output_file.good()) throw std::runtime_error("Could not open file: " + filename);

            //? READ DATA UNTIL BOUNDARY
			pos = _remaining_content.find(_boundary + "--");
			if (pos != std::string::npos)
			{
				output_file.write(_remaining_content.c_str(), pos);
				_remaining_content.clear();
			}
			else
				output_Content_Body(output_file);
		}

    }

    std::cout << B << "Boundary: " << _boundary<< "\n";
    std::cout << "Content-Disposition: " << contentDisposition << "\n";
    std::cout << "Filename: " << filename << "\n";
    std::cout << "Content-Type: " << contentType << C << "\n" << std::endl;
}


void Post::output_Content_Body(std::ofstream &output_file) {
	ssize_t		 bytes_read 	= 0;
	char		buffer[1024] = {0};
	std::string	old_buffer = _remaining_content;
	bool		is_second_iteration = !_remaining_content.empty();

	//TODO: supprimer le fichier en cas d'érreur

	//TODO: S'arréter à la boundary pour plusieurs images (stocker dans un buffer de réponse)
	while((bytes_read = recv(_client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {

		std::string full = old_buffer + std::string(buffer, bytes_read);
		std::size_t end = full.find(_boundary);
		if (end != std::string::npos)
		{
			std::cout << G << full.substr(end, full.size() - end) << C << std::endl; //? DEBUG
			output_file.write(full.c_str(), end);
			_remaining_content.assign(full, end, full.size() - end);
			is_second_iteration = false;
			break;
		}
		else output_file << old_buffer;

		_body_size += bytes_read;
		old_buffer.assign(buffer, bytes_read);
	}
	if (bytes_read == (ssize_t) -1) throw std::runtime_error("Receive failed");
	// if (bytes_read == 0) throw std::runtime_error("Connexion closed"); //? Necessary ?
	if (_body_size > _server.getBody()) throw std::runtime_error("Body too large");
	output_file.close();
}


std::string Post::receive_content_header(void) {
	ssize_t				bytes_read = 0;
    char				buffer = 0;
    std::ostringstream	content_stream;

	std::size_t pos = _remaining_content.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		content_stream << _remaining_content.substr(0, pos + 4);
		_remaining_content = _remaining_content.erase(0, pos + 4);
		return (content_stream.str());
	}

	while (content_stream.str().find("\r\n\r\n") == std::string::npos && (bytes_read = recv(_client_fd, &buffer, 1, 0) > 0)) {
		_body_size += bytes_read;
		content_stream << buffer;
	}
	if (bytes_read == (ssize_t) -1) throw std::runtime_error("Receive failed");
	// else if (bytes_read == 0) throw std::runtime_error("Connexion closed"); //? Necessary ?
	if (_body_size > _server.getBody()) throw std::runtime_error("Body too large");

	return (content_stream.str());
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */