#include "../includes/Request.hpp"

/*
** ------------------------------- STATIC -------------------------------------
*/

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(int &client_fd, Server &s) : _extension("None"), _accept(false) {
    char buffer[2];
    int bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) != 0) {
        buffer[bytes_received] = '\0';
        _httpRequest += buffer;
		if (_httpRequest.find("\r\n\r\n") != std::string::npos) break;
		if (bytes_received < 0) throw std::runtime_error("Receive failed");
    }

    if (bytes_received < 0) throw std::runtime_error("Receive failed");

    std::cout << "Message received: " << _httpRequest << std::endl;

    parse_request(s);

	if (_method == POST) {
		bytes_received = recv(client_fd, _content, sizeof(_content) - 1, 0);
		if (bytes_received < 0) throw std::runtime_error("Receive failed");
		_content[bytes_received] = '\0';
	}

    if (_location->getMethods((unsigned int)_method)) {
        _accept = true;
    }
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Request const & i ) {
	o << Y << "----Request-----\n";
	o << "Method: ";
	if (i.getMethod() == GET) o << "GET";
	else if (i.getMethod() == POST) o << "POST";
	else if (i.getMethod() == DELETE) o << "DELETE";
	else o <<  R "UNKNOWN" Y;
	o << "\n";
	o << "Extension: " << i.getExtension() << "\n";
	o << "Path: " << i.getPath() << "\n";
	o << "Accept: " << (i.isAccepted() ? "true" : "false") << "\n";
	o << "------End------\n" << C;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Request::resolvePath(Server &s) {
	if (_path == (_location->getRoot() + "/"))
		_path += _location->getIndex();
    else if (_extension == "None") {
        _path += ".html";
	}
}

void Request::parse_extension( void ) {
	// std::cout << R << _path << C << std::endl; //* DEBUG
	size_t start = _path.find('.', 1);
	if (start != std::string::npos && (start + 1) < _path.size() - 1) {
		start += 1;
		_extension = _path.substr(start, _path.size() - start);
	}
}

void Request::parse_request(Server &s){
	char path_buffer[BUFFER_SIZE];
	char type_buffer[BUFFER_SIZE];

	if (sscanf(this->_httpRequest.c_str(), "%s %s %*s", type_buffer, path_buffer) != 2)
		throw std::runtime_error("Error while parsing request");

	std::string request_path(path_buffer);
	std::string request_method (type_buffer);

    std::size_t pos;
    while ((pos = request_path.find("../")) != std::string::npos) {
        request_path.erase(pos, 3);
    }

	_location = s.getLocations().at(0);
	for (int i = 0; i < s.getLocations().size(); i++) {
		if (s.getLocations().at(i)->getLocation() == request_path) {
			_location = s.getLocations().at(i);
		}
	}

	this->_path = _location->getRoot() + request_path;

	if (request_method == "GET") this->_method = GET;
	else if (request_method == "POST") this->_method = POST;
	else this->_method = DELETE;
	parse_extension();
	resolvePath(s);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const char			*Request::getContent(void) const {return (_content);}
const std::string	&Request::getHttpRequest(void) const {return (_httpRequest);}
const short			&Request::getMethod(void) const {return (_method);}
const std::string	&Request::getExtension(void) const {return (_extension);}
const std::string 	&Request::getPath(void) const {return(_path);}
const bool			&Request::isAccepted(void) const {return(_accept);}
Location			*Request::getLocation(void) const { return (_location);}

/* ************************************************************************** */