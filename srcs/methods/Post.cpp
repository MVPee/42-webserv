#include "../../includes/methods/Post.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Post::Post(Request &request, Server &server) : 
_request(request), 
_server(server), 
_body_size(0), 
_status_code(request.get_status_code()),
_state(ReceivingHeader) {
	try {
		const std::string header = request.getHttpRequest();
		_content_length = get_data_in_header(header, "Content-Length: ", "\r\n");
		_content_type = get_data_in_header(header, "Content-Type: ", "\r\n");

		if (std::strtoul(_content_length.c_str(), 0, 10) > (unsigned long)server.getBody()) {
			throw_and_set_status(PAYLOAD_TOO_LARGE, "Body-size too long");
		}

		if (_content_type == "multipart/form-data")
			_boundary = get_data_in_header(header, "boundary=", "\r");
		// if (_content_type != "multipart/form-data" || _content_type != "plain/text")
		// 	throw_and_set_status(NOT_IMPLEMENTED, "Form is not a 'multipart/form-data' or 'plain/text'");

	}
	catch(const std::exception& e) {
		if (_status_code == OK) _status_code = ERROR_INTERNAL_SERVER;
		_state = Error;
		std::cerr << R << e.what() << C << '\n';
	}
	
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Post::~Post() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void Post::decide_action (std::string &new_content) {
	try {
		_body_size += new_content.size();
		if (_body_size > _server.getBody()) throw_and_set_status(PAYLOAD_TOO_LARGE, "Body size exceeded");

		if (_state == ReceivingHeader) {
			std::string total = _remaining_content + new_content;
			std::size_t pos;
			if (_content_type != "multipart/form-data" && std::strtoul(_content_length.c_str(), 0, 10) <= total.size())
			{
				_state = Completed;
			}
			else if ((pos = total.find(HEADER_DELIMITER)) == std::string::npos)
				_remaining_content = total;
			else
			{
				pos+= HEADER_SIZE;
				_state = HandlingBody;
				_header = total.substr(0, pos);
				_remaining_content = total.substr(pos, total.size());
				new_content.clear();
				get_file_infos();
			}
		}
		if (_state == HandlingBody)
			output_Content_Body(new_content);
		if (_remaining_content == _boundary + "--\r\n") {
			_state = Completed;
			clear_file_infos();
		}
	}
	catch(const std::exception& e) {
		if (_status_code == OK) _status_code = ERROR_INTERNAL_SERVER;
		clear_file_infos();
		_state = Error;
		std::cerr << R << e.what() << C << '\n';
	}
}

void Post::output_Content_Body(std::string &new_content) {
	std::string	old_buffer = _remaining_content;


	std::string full = old_buffer + new_content;
	std::size_t end = full.find(_boundary);
	if (end != std::string::npos) {
		_file.output_file.write(full.c_str(), end);
		_remaining_content = full.substr(end, full.size() - end);
		_state = ReceivingHeader;
	}
	else {
		_file.output_file << _remaining_content;
		_remaining_content = new_content;
	}
	if (!_file.output_file.good()) throw_and_set_status(ERROR_INTERNAL_SERVER, "Error when writing in the output file");

}

std::string Post::get_data_in_header(const std::string &header, const std::string &first_delimiter, const std::string &end_delimiter) {
	std::size_t start = header.find(first_delimiter);
	if (start != std::string::npos) {
		std::string ret = header.substr(start + first_delimiter.size());
		std::size_t end = ret.find_first_of(end_delimiter + ";");
		if (end != std::string::npos)
			return ret.substr(0, end);
	}
	throw_and_set_status(BAD_REQUEST, "'" + first_delimiter + "' missing");
	return "";
}

void Post::throw_and_set_status(const size_t status_code, std::string message) {
	_status_code = status_code;
	throw std::runtime_error(message);
}

void Post::get_file_infos(void) {
	clear_file_infos();
	_file.contentDisposition = get_data_in_header(_header, "Content-Disposition: ", ";");

	_file.filename = get_data_in_header(_header, "filename=\"", "\"");
	_file.filename = _request.getLocation()->getRoot() + _request.getLocation()->getUpload() + "/" + _file.filename;

    _file.contentType = get_data_in_header(_header, "Content-Type: ", "\r");
	if (_file.contentType.find("text/") == 0)
		throw_and_set_status(UNSUPPORTED_MEDIA_TYPE, "Posted content is not a file"); //? change that ?

	_file.output_file.open(_file.filename.c_str(), std::ofstream::trunc | std::ios::binary);
	if (!_file.output_file.is_open() || !_file.output_file.good()) throw_and_set_status(ERROR_INTERNAL_SERVER, "Couldn't open file");
	
}
void Post::clear_file_infos(void) {
	_file.contentDisposition.clear();
	_file.filename.clear();
    _file.contentType.clear();
	if (_file.output_file.is_open()) {
		_file.output_file.close();
		if (!_file.output_file.good() || (_status_code != OK && _status_code != NO_CONTENT))
			remove(_file.filename.c_str());
	}
	_file.output_file.clear();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */