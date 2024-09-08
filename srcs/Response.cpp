#include "../includes/Response.hpp"

/*
** ------------------------------- STATIC --------------------------------
*/

static std::string get_content_type(std::string extension) {
    std::map<std::string, std::string> extensions;
    extensions["html"] = "text/html";
    extensions["php"] = "text/php";
    extensions["ico"] = "image/x-icon";
    extensions["png"] = "image/png";
    extensions["jpg"] = "image/jpg";
    extensions["None"] = "text/html";

	if (extensions.count(extension)) return (extensions[extension]);
	else return "text/html";
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

const std::string Response::generate_response(const std::string &page_content) const {
    std::string content_type = _status_code < 400 ? get_content_type(_extension) : "text/html";
	const std::string status_message = get_status_message(_status_code);

	std::string response = ft_to_string(HTML_VERSION) + " " + ft_to_string(_status_code) + " " + status_message + "\r\n" \
								+ "Content-Type: " + content_type + "\r\n" \
								+ "Content-Length: " + ft_to_string(page_content.size()) + "\r\n"\
								+ "Connection: close" \
								+ ((_extension == "redirection") ? ("\r\nLocation: " + _location->getRedirection() + "\r\n") : ("")) \
								+ "\r\n\r\n" + page_content + '\0';
	return (response);
}

Response::Response(const Server &s, std::string full_request) : _httpRequest(full_request) {
	//Ancien REQUEST
	try {
		parse_request(s);
		if (_location->getMethods((unsigned int)_method) || _extension == "redirection") {
			_accept = true;
			_status_code = (_method == POST || _method == DELETE) ? NO_CONTENT : OK;
			if (_extension == "redirection") _status_code = REDIRECTION_PERMANENTLY;
		}
	}
	catch (const std::exception &e) {
		if (_status_code == OK) _status_code = ERROR_INTERNAL_SERVER;
		std::cerr << R << e.what() << C << '\n';
	}
	//ANCIEN RESPONSE
	std::string response_header;

    if (_method == POST && _accept) {
		// Post Form(sd, request, server);
		;
	}
	if (_method == DELETE) {
		if (_accept) {
			if (access(_path.c_str(), F_OK) != 0)
				_status_code = ERROR_NOT_FOUND;
			else if (remove(_path.c_str()))
				_status_code = ERROR_INTERNAL_SERVER;
			else 
				_status_code = NO_CONTENT;
		}
	}
	else
		response_header = Get (*this, s).get_content();

	_httpResponse = generate_response(response_header);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Response const & i ) {
	(void)i;
	//o << "Value = " << i.getValue();
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Response::resolvePath(const Server &s) {
	struct stat info;

	if (!_location->getRedirection().empty())
		_extension = "redirection";
	else if (stat(_path.c_str(), &info) == 0) {
		if (info.st_mode & S_IFDIR) {
			_extension = "directory";
			if (_path[_path.size() - 1] != '/')
				_path += '/';
		}
	}
	if (_path == (_location->getRoot() + "/") || _extension == "directory") {
		if (_path == (_location->getRoot() + "/") && _location->getIndex() != ""){
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

void Response::parse_extension( void ) {
	// std::cout << R << _path << C << std::endl; //* DEBUG
	size_t start = _path.find('.', 1);
	if (start != std::string::npos && (start + 1) < _path.size() - 1) {
		start += 1;
		_extension = _path.substr(start, _path.size() - start);
	}
}

void Response::parse_request(const Server &s) {
	char path_buffer[BUFFER_SIZE];
	char type_buffer[BUFFER_SIZE];
	char http_version[BUFFER_SIZE];

	if (sscanf(this->_httpRequest.c_str(), "%s %s %s", type_buffer, path_buffer, http_version) != 3)
		throw_and_set_status(BAD_REQUEST, "Error while parsing request");

	std::string request_path(path_buffer);
	std::string request_method (type_buffer);

	if (std::string(http_version) != "HTTP/1.1")
		throw_and_set_status(HTTP_VERSION_NOT_SUPPORTED, "Not supported http version");

    std::size_t pos;
    while ((pos = request_path.find("../")) != std::string::npos)
        request_path.erase(pos, 3);

	if ((pos = request_path.find("?")) != std::string::npos)
		request_path.erase(pos);

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
		_path = _location->getRoot() + request_path;
	}
	else
		_path = _location->getRoot() + request_path.erase(0, _location->getLocation().size());


	if (request_method == "GET") _method = GET;
	else if (request_method == "POST") _method = POST;
	else _method = DELETE;
	parse_extension();
	resolvePath(s);
}

void Response::throw_and_set_status(const size_t status_code, std::string message) {
	_status_code = status_code;
	throw std::runtime_error(message);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */