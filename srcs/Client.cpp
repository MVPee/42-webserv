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
_state(ReceivingHeader) {

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client() {
	delete _request;
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void Client::handle_client( void )
{
	if (!_request)
	{
		_request = new Request(_header, _server);
	}
	if (_request->getMethod() == GET || _request->getMethod() == DELETE)
	{
		Response(_client_fd, *_request, _server, _client_fd);
	}
}


void Client::receive_content( void )
{
	char buffer[BUFFER_SIZE];
	ssize_t bytes_received;

	bytes_received = recv(_client_fd, &buffer, sizeof(buffer) - 1, 0);
	if (bytes_received == 0) _state = Completed;
	else if (bytes_received > 0)
	{
		std::string received_content(buffer, bytes_received);
		std:size_t pos;
		if (_state == ReceivingHeader && (pos = received_content.find(HEADER_DELIMITER)) == std::string::npos)
		{
			_header += received_content;
		}
		else
		{
			if (_state == ReceivingHeader)
			{
				_state = HandlingBody;
				_header += received_content.substr(0, pos);
				_body = received_content.substr(pos, received_content.size());
			}
			_body += received_content;
		}
	}
}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */