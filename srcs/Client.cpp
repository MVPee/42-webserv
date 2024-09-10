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

void Client::handle_client( void )
{
	if (_state == HandlingBody && _request->getExtension() != "cgi")
	{
		if (_request->getMethod() != POST || _post->get_state() == Completed)
		{
			std::string response_data = Response(_client_fd, *_request, _server, _client_fd).getResponse();
			//! send ici
			ssize_t bytes_sent = send(_client_fd, response_data.c_str(), response_data.size(), 0); //TODO protéger le send
			_state = Completed;
		}
	}
	else if (_state == HandlingBody && _request->getExtension() == "cgi")
	{
		std::cout << Cgi(*_request) << std::endl;
		_state = Completed;
	}
}


void Client::receive_content( void )
{
	char buffer[BUFFER_SIZE];
	ssize_t bytes_received;

	bytes_received = recv(_client_fd, &buffer, sizeof(buffer) - 1, 0); //TODO Check for -1
	if (bytes_received == (ssize_t) 0) _state = Completed; //! Connexion closed
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
	if (_state == HandlingBody && !_request)
		_request = new Request(_header, _server);
	if (_state == HandlingBody && _request->getMethod() == POST && _request->getExtension() != "cgi") {
		if (!_post)
			_post = new Post(_client_fd, *_request, _server);
		_post->decide_action(_body);
	}
}

void Client::clear(void) {
	if (_client_fd) {
		close(_client_fd);
		_client_fd = 0;
		_header.clear();
		_body.clear();
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