#include "../../includes/methods/Post.hpp"

std::string get_data_in_header(std::string &header, std::string first_delimiter, std::string end_delimiter)
{
	std::size_t start = header.find(first_delimiter);
	if (start == std::string::npos) return (std::string());

	std::string ret = header.substr(start + first_delimiter.size());

	std::size_t end = ret.find(end_delimiter);
	if (end == std::string::npos) return (std::string());

	return (ret.substr(0, end));
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Post::Post(const int client_fd, Request &request, Server &server) : 
_client_fd(client_fd), 
_request(request), 
_server(server), 
_body_size(0), 
_status_code(OK) {
	try{
		std::string header = request.getHttpRequest();
		_boundary = get_data_in_header(header, "boundary=", "\r");
		std::string content_length = get_data_in_header(header, "Content-Length: ", "\r");
		std::string	content_type = get_data_in_header(header, "Content-Type: ", ";");

		if (_boundary.empty() || content_length.empty() || content_type.empty())
			throw_and_set_status(ERROR_INTERNAL_SERVER, "boundary or content-length not found in header");

		if (std::strtoul(content_length.c_str(), 0, 10) > server.getBody()) {
			while (recv(client_fd, 0, 255, 0) == 255);
			throw_and_set_status(PAYLOAD_TOO_LARGE, "Body-size too long");
		}

		if (content_type != "multipart/form-data")
			throw_and_set_status(NOT_IMPLEMENTED, "Form is not a multipart/form-data");

		while (_remaining_content != _boundary + "--\r\n") { //? check if this cause an issue
			handle_post_request(request.getLocation());
		}
	}
	catch(const std::exception& e)
	{
		if (_status_code == OK) _status_code = ERROR_INTERNAL_SERVER;
		std::cerr << R << e.what() << C << '\n';
	}
	
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Post::~Post() {
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Post const & i ) {
	(void)i;
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void Post::handle_post_request(Location *location) {
    std::string body_header (receive_content_header());


	std::string contentDisposition = get_data_in_header(body_header, "Content-Disposition: ", ";");
	if (contentDisposition.empty()) throw_and_set_status(BAD_REQUEST, "Content-disposition missing");

	std::string filename = get_data_in_header(body_header, "filename=\"", "\"");
	//? FOR DAN: RM si plus besoin
	if (filename.empty()) throw_and_set_status(BAD_REQUEST, "filename missing");
	filename = location->getRoot() + location->getUpload() + "/" + filename;
	std::cout << B << filename << C << std::endl; //? DEBUG

    std::string contentType = get_data_in_header(body_header, "Content-Type: ", "\r");
	if (contentType.empty()) throw_and_set_status(BAD_REQUEST, "Content-Type missing");

	std::ofstream output_file(std::string(filename).c_str(), std::ios::trunc | std::ios::binary);
	if (!output_file.is_open() || !output_file.good()) throw_and_set_status(ERROR_INTERNAL_SERVER, "Couldn't open file");


	std::size_t pos = _remaining_content.find(_boundary);
	if (pos != std::string::npos)
	{
		output_file.write(_remaining_content.c_str(), pos);
		_remaining_content.erase(0, pos);

		if (!output_file.good())
		{
			remove(filename.c_str());
			throw_and_set_status(ERROR_INTERNAL_SERVER, "Error with the output file");
		}
	}
	else
		output_Content_Body(output_file, filename);

	//? DEBUG PRINTS
    // std::cout << B << "Boundary: " << _boundary<< "\n";
    // std::cout << "Content-Disposition: " << contentDisposition << "\n";
    // std::cout << "Filename: " << filename << "\n";
    // std::cout << "Content-Type: " << contentType << C << "\n" << std::endl;
}


void Post::output_Content_Body(std::ofstream &output_file, std::string &filename) {
	ssize_t		 bytes_read 	= 0;
	char		buffer[1024]	= {0};
	std::string	old_buffer = _remaining_content;


	while(_body_size <= _server.getBody() && (bytes_read = recv(_client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) { //? What if no delimiter ?

		std::string full = old_buffer + std::string(buffer, bytes_read);
		std::size_t end = full.find(_boundary);
		if (end != std::string::npos)
		{
			output_file.write(full.c_str(), end);
			_remaining_content = full.substr(end, full.size() - end);
			break;
		}
		else output_file << old_buffer;

		_body_size += bytes_read;
		old_buffer.assign(buffer, bytes_read);
	}

	output_file.close();

	if (!output_file.good() || bytes_read == (ssize_t) -1 || bytes_read == 0 | _body_size > _server.getBody())
		remove(filename.c_str());

	if (bytes_read == (ssize_t) -1) throw_and_set_status(ERROR_INTERNAL_SERVER, "Recv failed");
	else if (bytes_read == 0) throw_and_set_status(CLIENT_CLOSED_REQUEST, "Connexion closed");
	else if (_body_size > _server.getBody()) throw_and_set_status(PAYLOAD_TOO_LARGE, "Body size exceeded");
	else if (!output_file.good()) throw_and_set_status(ERROR_INTERNAL_SERVER, "Error with the output file");

}


std::string Post::receive_content_header(void) {
	ssize_t				bytes_read = 0;
    char				buffer = 0;
    std::ostringstream	content_stream;

	if(!_remaining_content.empty()) {
		std::size_t pos = _remaining_content.find("\r\n\r\n");
		if (pos != std::string::npos) {
			content_stream << _remaining_content.substr(0, pos + 4);
			_remaining_content.erase(0, pos + 4);
			return (content_stream.str());
		}

		content_stream << _remaining_content;
		_remaining_content.clear();
	}

	while (_body_size <= _server.getBody() && content_stream.str().find("\r\n\r\n") == std::string::npos \
	&& (bytes_read = recv(_client_fd, &buffer, 1, 0) > 0)) { //? What if no delimiter ?
		_body_size += bytes_read;
		content_stream << buffer;
	}

	if (bytes_read == (ssize_t) -1) throw_and_set_status(ERROR_INTERNAL_SERVER, "Recv failed");
	else if (bytes_read == 0) throw_and_set_status(CLIENT_CLOSED_REQUEST, "Connexion closed");
	else if (_body_size > _server.getBody()) throw_and_set_status(PAYLOAD_TOO_LARGE, "Body size exceeded");

	return (content_stream.str());
}

void Post::throw_and_set_status(size_t status_code, std::string message)
{
	_status_code = status_code;
	throw std::runtime_error(message);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const size_t Post::get_status_code( void ) const {return (_status_code);};


/* ************************************************************************** */