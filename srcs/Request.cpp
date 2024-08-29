#include "../includes/Request.hpp"

/*
** ------------------------------- STATIC -------------------------------------
*/

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(int &client_fd, Server &s) : _fd(0), _extension("None") {
	_fd = recv(client_fd, _buffer, sizeof(_buffer), 0);
	if (_fd < 0) throw std::runtime_error("Receive failed");
	// std::cout << "Message received: " << _buffer << std::endl; //* DEBUG
	parse_request(s);
	resolvePath(s);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {
	if (_fd >= 0)
		close(_fd);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Request const & i ) {
	o << Y << "\n----Request-----\n";
	o << "Method: ";
	if (i.getMethod() == GET) o << "GET";
	else if (i.getMethod() == POST) o << "POST";
	else if (i.getMethod() == DELETE) o << "DELETE";
	else o <<  R "UNKNOWN" Y;
	o << "\n";
	o << "Extension: " << i.getExtension() << "\n";
	o << "Path: " << i.getPath() << "\n";
	o << "------End------\n" << C;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Request::resolvePath(Server &s) {
	if (_path == (s.getRoot() + "/"))
		_path += s.getIndex();
    else if (_extension == "None") {
        _path += ".html";
	}
}

void Request::parse_extension( void ) {
	std::cout << R << _path << C << std::endl;
	size_t start = _path.find('.', 1);
	if (start != std::string::npos && (start + 1) < _path.size() - 1) {
		start += 1;
		_extension = _path.substr(start, _path.size() - start);
	}
}

void Request::parse_request(Server &s){
	char path_buffer[1024];
	char type_buffer[1024];

	if (sscanf(this->_buffer, "%s %s %*s", type_buffer, path_buffer) != 2)
		throw std::runtime_error("Error while parsing request");

	std::string request_path(path_buffer);
	std::string request_method (type_buffer);

    std::size_t pos;
    while ((pos = request_path.find("../")) != std::string::npos) {
        request_path.erase(pos, 3);
    }

	this->_path = s.getRoot() + request_path;

	if (request_method == "GET") this->_method = GET;
	else if (request_method == "POST") this->_method = POST;
	else this->_method = DELETE;
	parse_extension();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const char 			*Request::getBuffer(void) const {return (_buffer);}
const int 			&Request::getFd(void) const {return (_fd);}
const short			&Request::getMethod(void) const {return (_method);}
const std::string	&Request::getExtension(void) const {return (_extension);}
const std::string 	&Request::getPath(void) const {return(_path);}

/* ************************************************************************** */