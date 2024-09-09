#include "../../includes/methods/Post.hpp"

std::string get_data_in_header(std::string &header, std::string first_delimiter, std::string end_delimiter) {
	std::size_t start = header.find(first_delimiter);
	if (start == std::string::npos)
		return (std::string());
	std::string ret = header.substr(start + first_delimiter.size());
	std::size_t end = ret.find(end_delimiter);
	if (end == std::string::npos)
		return (std::string());
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
_status_code(request.get_status_code()),
_state(ReceivingHeader) {
	try {
		std::string header = request.getHttpRequest();
		_boundary = get_data_in_header(header, "boundary=", "\r");
		std::string content_length = get_data_in_header(header, "Content-Length: ", "\r");
		std::string	content_type = get_data_in_header(header, "Content-Type: ", ";");

		if (_boundary.empty() || content_length.empty() || content_type.empty())
			throw_and_set_status(BAD_REQUEST, "boundary or content-length not found in header");

		if (std::strtoul(content_length.c_str(), 0, 10) > server.getBody()) {
			// fcntl(client_fd, F_SETFL, O_NONBLOCK);
			// while (recv(client_fd, 0, 255, 0) > 0); //* still a problem when uploading big files and exceeding body size limit (bad response)
			throw_and_set_status(PAYLOAD_TOO_LARGE, "Body-size too long");
		}

		if (content_type != "multipart/form-data")
			throw_and_set_status(NOT_IMPLEMENTED, "Form is not a multipart/form-data");

	}
	catch(const std::exception& e) {
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

void Post::decide_action ( std::string &new_content)
{
	if (_state == ReceivingHeader)
	{
		std::string total = _remaining_content + new_content;
		_body_size += new_content.size();
		std::size_t pos;
		if ((pos = total.find(HEADER_DELIMITER)) == std::string::npos)
			_remaining_content = total;
		else
		{
			pos+= HEADER_SIZE;
			_state = HandlingBody;
			_header = total.substr(0, pos);
			_remaining_content = total.substr(pos, total.size());
			new_content.clear();
		}
	}
	if (_state == HandlingBody)
	{
		_body_size += new_content.size();
		handle_post_request(_request.getLocation(), new_content);
	}
	if (_remaining_content == _boundary + "--\r\n")
		_state = Completed;

}

void Post::handle_post_request(Location *location, std::string &new_content) {
    std::string body_header (_header);


	std::string contentDisposition = get_data_in_header(body_header, "Content-Disposition: ", ";");
	if (contentDisposition.empty()) throw_and_set_status(BAD_REQUEST, "Content-disposition missing");

	std::string filename = get_data_in_header(body_header, "filename=\"", "\"");
	if (filename.empty()) throw_and_set_status(BAD_REQUEST, "filename missing");
	filename = location->getRoot() + location->getUpload() + "/" + filename;
	// std::cout << B << filename << C << std::endl; //* DEBUG

    std::string contentType = get_data_in_header(body_header, "Content-Type: ", "\r");
	if (contentType.empty()) throw_and_set_status(BAD_REQUEST, "Content-Type missing");

	std::ofstream output_file(std::string(filename).c_str(), std::ofstream::app | std::ios::binary);
	if (!output_file.is_open() || !output_file.good()) throw_and_set_status(ERROR_INTERNAL_SERVER, "Couldn't open file");


	std::size_t pos = _remaining_content.find(_boundary);
	if (pos != std::string::npos)
	{
		std::cout << G << "woui yé suis passé issi" C << std::endl;
		output_file.write(_remaining_content.c_str(), pos);
		_remaining_content.erase(0, pos);

		if (!output_file.good())
		{
			remove(filename.c_str());
			throw_and_set_status(ERROR_INTERNAL_SERVER, "Error with the output file");
		}
		_remaining_content += new_content;
	}
	else
		output_Content_Body(output_file, filename, new_content);

	//? DEBUG PRINTS
    // std::cout << B << "Boundary: " << _boundary<< "\n";
    // std::cout << "Content-Disposition: " << contentDisposition << "\n";
    // std::cout << "Filename: " << filename << "\n";
    // std::cout << "Content-Type: " << contentType << C << "\n" << std::endl;
}


void Post::output_Content_Body(std::ofstream &output_file, std::string &filename, std::string &new_content) {
	std::string	old_buffer = _remaining_content;


	std::string full = old_buffer + new_content;
	std::size_t end = full.find(_boundary);
	if (end != std::string::npos)
	{
		output_file.write(full.c_str(), end);
		_remaining_content = full.substr(end, full.size() - end);
	}
	else
	{
		output_file << _remaining_content;
		_remaining_content = new_content;
	}


	output_file.close();

	if (!output_file.good() || _body_size > _server.getBody())
		remove(filename.c_str());


	if (_body_size > _server.getBody()) throw_and_set_status(PAYLOAD_TOO_LARGE, "Body size exceeded");
	else if (!output_file.good()) throw_and_set_status(ERROR_INTERNAL_SERVER, "Error with the output file");

}


void Post::throw_and_set_status(const size_t status_code, std::string message)
{
	_status_code = status_code;
	throw std::runtime_error(message);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/



/* ************************************************************************** */