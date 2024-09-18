#include "../../includes/methods/Get.hpp"


const char* get_status_message(const size_t status_code) {
	switch (status_code) {
		case OK:                                return "OK";
		case REDIRECTION_PERMANENTLY:           return "Moved Permanently";
		case BAD_REQUEST:                       return "Bad Request";
		case ERROR_NOT_FOUND:                   return "Not Found";
		case ERROR_REQUEST_TIMEOUT:             return "Request Timeout";
		case PAYLOAD_TOO_LARGE:                 return "Payload Too Large";
		case CLIENT_CLOSED_REQUEST:             return "Client Closed Request";
		case ERROR_INTERNAL_SERVER:             return "Internal Server Error";
		case NOT_IMPLEMENTED:                   return "Not Implemented";
		case HTTP_VERSION_NOT_SUPPORTED:        return "HTTP Version Not Supported";
		case FORBIDDEN:                         return "Forbidden";
		case NO_CONTENT:                        return "No Content";
		case UNSUPPORTED_MEDIA_TYPE:            return "Unsupported Media Type";
		default:                                return "Unknown Status Code";
	}
}


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Get::Get(Request &request, Server &server) :  
_request(request), 
_server(server),
_status_code(request.get_status_code()) {
    if ((!request.isAccepted() && _status_code != BAD_REQUEST) || request.getExtension() == "directory")
        _status_code = FORBIDDEN;
    else if (request.getExtension() == "redirection")
        generate_redirection(request.getLocation()->getRedirection());
    else if (request.getExtension() == "listing")
        generate_listing();
    if (_content.empty())
        get_file();
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Get::~Get() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Get const & i ) {
	(void)i;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Get::generate_redirection(std::string redirection) {
    _content = ft_to_string(get_status_message(_status_code)) + ". Redirecting to " + redirection;
}

void Get::generate_listing(void) {
    struct dirent* entry;
    std::string listing_content;
    std::string dirname;
    std::string absolutePath;
    std::string path = _request.getPath();

    DIR* dir = opendir(_request.getPath().c_str());
    if (dir == NULL) {
        _status_code = ERROR_NOT_FOUND;
        return;
    }

    if (_request.getLocation()->getLocation() == "/")
        absolutePath = path.erase(0, _request.getLocation()->getRoot().size());
    else
        absolutePath = _request.getLocation()->getLocation() + path.erase(0, _request.getLocation()->getRoot().size());

    while ((entry = readdir(dir)) != NULL) {
        dirname = entry->d_name;
        if (entry->d_type == DT_DIR)
            listing_content += "<a href=\"" + absolutePath + dirname + "\">" + dirname + "/" + "</a><br>" + '\n';
        if (entry->d_type == DT_REG)
            listing_content += "<a href=\"" + absolutePath + dirname + "\">" + dirname + "</a><br>" + '\n';
    }
    _content = listing_content;
    std::cout << B << _content << C << std::endl; //? DEBUG
    closedir(dir);
}

void Get::get_file() {
    std::ifstream file;

	if (_status_code == OK) {
		file.open(_request.getPath().c_str());
		if (!file.is_open() || !file.good())
			_status_code = ERROR_NOT_FOUND;
		else 
			_content = ft_to_string(file.rdbuf());
	}

	if (_status_code >= 400 && _request.getLocation()) {
		file.open(ft_to_string(_request.getLocation()->getRoot() + '/' + _request.getLocation()->getErrorPage(_status_code)).c_str());
		_content = ft_to_string(file.rdbuf());
	}
    if (!_content.empty())
		file.close();
	else 
		_content = "<h1>default: " + ft_to_string(_status_code) + " " + get_status_message(_status_code) + "</h1>";
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string &Get::get_content(void) const { return(_content);};

/* ************************************************************************** */