#include "../includes/Request.hpp"


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(int &client_fd, Server &s) : _fd(0) {
	_fd = recv(client_fd, _buffer, sizeof(_buffer), 0);
	if (_fd < 0) throw std::runtime_error("Receive failed");
	// std::cout << "Message received: " << _buffer << std::endl; //* DEBUG
	parse_request(s);
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
	o << Y << "\n----Request-----\n";
	o << "Type: ";
	if (i.getType() == GET) o << "GET";
	else if (i.getType() == POST) o << "POST";
	else if (i.getType() == DELETE) o << "DELETE";
	else o <<  R "UNKNOWN" Y;
	o << "\n";
	o << "Path: " << i.getPath() << "\n";
	o << "------End------\n" << C;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Request::parse_request(Server &s){
	char path_buffer[1024];
	char type_buffer[1024];

	if (sscanf(this->_buffer, "%s %s %*s", type_buffer, path_buffer) != 2)
		throw std::runtime_error("Error while parsing request");

	std::string request_path(path_buffer);
	std::string request_type (type_buffer);

    std::size_t pos;
    while ((pos = request_path.find("../")) != std::string::npos) {
        request_path.erase(pos, 3);
    }

	this->_path = s.getRoot() + request_path;

	if (request_type == "GET") this->_type = GET;
	else if (request_type == "POST") this->_type = POST;
	else this->_type = DELETE;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const char 			*Request::getBuffer(void) const {return (_buffer);}
const int 			&Request::getFd(void) const {return (_fd);}
const short			&Request::getType(void) const {return (_type);}
const std::string 	&Request::getPath(void) const {return(_path);}

/* ************************************************************************** */