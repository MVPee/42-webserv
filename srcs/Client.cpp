#include "../includes/Client.hpp"

/*
** ------------------------------- STATIC -------------------------------------
*/

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client(Server &s, int fd) :
_server(s),
_client_fd(fd),
_state(ReceivingHeader),
_request(0),
_post(0) {

}

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

void Client::response( void )
{
	if (_state == HandlingBody)
	{
		if (_request->getMethod() != POST || _post->get_state() == Completed)
		{
			if (_response.empty())
				_response = Response(_client_fd, *_request, _server, _client_fd).getResponse();
			ssize_t bytes_sent = send(_client_fd, _response.c_str(), _response.size(), 0);
			if (bytes_sent < 0 || bytes_sent == _response.size()) clear();
			else if (bytes_sent < _response.size())
				_response = _response.substr(bytes_sent);
		}
	}
}


void Client::request( void )
{
	receive_content();

	if (_state == HandlingBody && !_request)
		_request = new Request(_header, _server);
	if (_state == HandlingBody && _request->getMethod() == POST && _request->getExtension() != "cgi") {
		if (!_post)
			_post = new Post(_client_fd, *_request, _server);
		_post->decide_action(_body);
	}
}



void Client::receive_content( void )
{

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
				_body = total.substr(pos, total.size());
			}
		}
		else
			_body = std::string(buffer, bytes_received);
	}
	//TODO handle failed receive

}

void Client::clear(void) {
	if (_client_fd) {
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
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */