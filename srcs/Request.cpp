#include "../includes/Request.hpp"

/*
** ------------------------------- STATIC -------------------------------------
*/

static void decode_path(std::string &path) {
	std::string new_path;
	size_t i = 0;
	while (i < path.size()) {
		if (path[i] == '%' && i + 2 < path.size() &&
			std::isxdigit(path[i + 1]) && std::isxdigit(path[i + 2])) {
			int letter_int;
			letter_int = strtol(path.substr(i + 1, 2).c_str(), 0, 16);
			new_path += static_cast<char>(letter_int);
			i += 3;
		}
		else new_path += path[i++];
	}
	path = new_path;
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::string &header, Server &s) : 
_accept(false),
_httpRequest(header),
_extension("None"), 
_status_code (FORBIDDEN),
_success(true),
_location(NULL) {
	try {

		parse_request(s);

		if (_location->getMethods((unsigned int)_method) || _extension == "redirection") {
			_accept = true;
			_status_code = (_method == POST || _method == DELETE) ? NO_CONTENT : OK;
			if (_extension == "redirection") _status_code = REDIRECTION_PERMANENTLY;
		}
	}
	catch(const std::exception& e) {
		if (_status_code == OK) _status_code = ERROR_INTERNAL_SERVER;
		std::cerr << R << e.what() << C << '\n';
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

std::ostream &operator<<(std::ostream &o, const Request &i) {
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

void Request::resolvePath(void) {
	struct stat info;

	if (find_cgi_extension(_path, _location->getCGI()) != std::string::npos) _extension = "cgi";
	if (!_location->getRedirection().empty()) _extension = "redirection";
	else if (stat(_path.c_str(), &info) == 0) {
		if (info.st_mode & S_IFDIR) {
			_extension = "directory";
			if (_path[_path.size() - 1] != '/')
				_path += '/';
		}
	}
	if (_path == (_location->getRoot() + "/") || _extension == "directory") {
		if (_path == (_location->getRoot() + "/") && _location->getIndex() != "") {
			_path += _location->getIndex();
			_extension = ".html";
		}
		else if (_location->getListing())
			_extension = "listing";
	}
    else if (_extension == "None") {
		if (_path[_path.size() - 1] != '/')
        	_path += ".html";
	}
}

void Request::parse_extension(void) {
	// std::cout << R << _path << C << std::endl; //* DEBUG
	size_t start = _path.find('.', 1);
	if (start != std::string::npos && (start + 1) < _path.size() - 1) {
		start += 1;
		_extension = _path.substr(start, _path.size() - start);
	}
}

void Request::parse_request(Server &s) {
	char path_buffer[BUFFER_SIZE];
	char type_buffer[BUFFER_SIZE];
	char http_version[BUFFER_SIZE];

	if (sscanf(this->_httpRequest.c_str(), "%s %s %s", type_buffer, path_buffer, http_version) != 3)
		throw_and_set_status(BAD_REQUEST, "Error while parsing request");

	std::string request_path(path_buffer);
	std::string request_method (type_buffer);

	// if (std::string(http_version) != "HTTP/1.1")
	// 	throw_and_set_status(HTTP_VERSION_NOT_SUPPORTED, "Not supported http version");

	decode_path(request_path);

    std::size_t pos = request_path.find("../");
    while (pos != std::string::npos) {
		pos = request_path.find("../");
        request_path.erase(pos, 3);
	}

	// if ((pos = request_path.find("?")) != std::string::npos)
	// 	request_path.erase(pos);

	_location = s.getLocations().at(0);
    for (size_t i = 0; i < s.getLocations().size(); i++) {
		Location* loc = s.getLocations().at(i);
		
		if (request_path.compare(0, loc->getLocation().size(), loc->getLocation()) == 0)
			if (request_path.size() == loc->getLocation().size() || request_path[loc->getLocation().size()] == '/')
				if (!_location || loc->getLocation().size() > _location->getLocation().size())
					_location = loc;
	}

	//? Get the good path
	if (!_location || _location->getLocation() == std::string("/")) {
        _location = s.getLocations().at(0);
		this->_path = _location->getRoot() + request_path;
	}
	else
		this->_path = _location->getRoot() + request_path.erase(0, _location->getLocation().size());


	if (request_method == "GET") this->_method = GET;
	else if (request_method == "POST") this->_method = POST;
	else this->_method = DELETE;
	parse_extension();
	resolvePath();
}


void Request::throw_and_set_status(const size_t set_status_code, const std::string message) {
	_status_code = set_status_code;
	// std::cout << G << _status_code << C << std::endl; //* Debug
	throw std::runtime_error(message);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const char			*Request::getContent(void) const { return _content; }
const std::string	&Request::getHttpRequest(void) const { return _httpRequest; }
const short			&Request::getMethod(void) const { return _method; }
const std::string	&Request::getExtension(void) const { return _extension; }
const std::string 	&Request::getPath(void) const { return _path; }
const bool			&Request::isAccepted(void) const { return _accept; }
Location			*Request::getLocation(void) const { return _location; }
size_t 				&Request::get_status_code(void) { return _status_code; }
const bool			&Request::getSuccess(void) const { return _success; }

/* ************************************************************************** */