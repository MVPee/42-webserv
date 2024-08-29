#include "../includes/Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(int &client_fd) : _fd(0) {
	_fd = recv(client_fd, _buffer, sizeof(_buffer), 0);
	if (_fd < 0) throw std::runtime_error("Receive failed");
	std::cout << "Message received: " << _buffer << std::endl;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {
	close(_fd);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Request const & i ) {
	(void)i;
	//o << "Value = " << i.getValue();
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

char *Request::getBuffer(void) {
	return (_buffer);
}

int &Request::getFd(void) {
	return (_fd);
}

/* ************************************************************************** */