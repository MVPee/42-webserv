#include "../includes/Client.hpp"

/*
** ------------------------------- STATIC -------------------------------------
*/
//? Temporary
static std::string get_data_in_header(const std::string &header, const std::string &first_delimiter, const std::string &end_delimiter) {
	std::size_t start = header.find(first_delimiter);
	if (start != std::string::npos) {
		std::string ret = header.substr(start + first_delimiter.size());
		std::size_t end = ret.find(end_delimiter);
		if (end != std::string::npos)
			return ret.substr(0, end);
	}
	return "";
}
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client(Server &s, int fd) :
_server(s),
_client_fd(fd),
_state(ReceivingHeader),
_request(0),
_post(0) {}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client() {
	clear();
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void Client::response(void) {
	if (_state == HandlingBody) {
		if (_request->getExtension() == "cgi") {
			//? temporary
			size_t len = std::strtoul(get_data_in_header(_header, "Content-Length: ", "\r").c_str(), 0, 10);
			if ((_request->getMethod() == POST && len >= _body.size()) || (_request->getMethod() == GET))
				_response = Response(*this).getResponse();
		}
		else if (_request->getMethod() != POST || _post->get_state() == Completed) {
			if (_response.empty())
				_response = Response(*this).getResponse();
		}
		if (!_response.empty()) {
			ssize_t bytes_sent = send(_client_fd, _response.c_str(), _response.size(), 0);
			if (bytes_sent < 0 || bytes_sent == _response.size())
			{
				_state = Completed;
				clear();
			}
			else if (bytes_sent < _response.size())
				_response = _response.substr(bytes_sent);
		}
	}
}


void Client::request(void) {
	char buffer[BUFFER_SIZE];
	ssize_t bytes_received;

	bytes_received = recv(_client_fd, &buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= (ssize_t) 0) {
		if (bytes_received <= (ssize_t) 0)
			std::cout << _client_fd << " close connection." << std::endl;
		clear();
	}
	else if (bytes_received > (ssize_t) 0) {
		if (_state == ReceivingHeader) {
			std:size_t pos;
			std::string total = _header + std::string(buffer, bytes_received);
			if ((pos = total.find(HEADER_DELIMITER)) == std::string::npos)
				_header += buffer;
			else {
				pos += HEADER_SIZE;
				_state = HandlingBody;
				_header = total.substr(0, pos);
				_body += total.substr(pos, total.size());
			}
		}
		else
			_body = std::string(buffer, bytes_received);
	}
	if (_state == HandlingBody && !_request)
		_request = new Request(_header, _server);
	if (_state == HandlingBody && _request->getMethod() == POST && _request->getExtension() != "cgi") {
		if (!_post)
			_post = new Post(_client_fd, *_request, _server);
		_post->decide_action(_body);
		_body.clear();
	}
}

void Client::clear(void) {
	if (_client_fd)
		close(_client_fd);
	_client_fd = 0;
	_header.clear();
	_body.clear();
	_response.clear();
	_state = ReceivingHeader;
	_connection_time = -1;
	if (_request) {
		delete _request;
		_request = 0;
	}
	if (_post) {
		delete _post;
		_post = 0;
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */