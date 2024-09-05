#include "../../includes/methods/Get.hpp"


const char* get_status_message(const size_t status_code) {
    switch (status_code) {
        case OK:                                return "OK";
        case BAD_REQUEST:                       return "BAD REQUEST";
        case ERROR_NOT_FOUND:                   return "NOT FOUND";
        case ERROR_REQUEST_TIMEOUT:             return "REQUEST TIMEOUT";
        case PAYLOAD_TOO_LARGE:                 return "PAYLOAD TOO LARGE";
        case CLIENT_CLOSED_REQUEST:             return "CLIENT CLOSED REQUEST";
        case ERROR_INTERNAL_SERVER:             return "INTERNAL SERVER ERROR";
        case NOT_IMPLEMENTED:                   return "NOT IMPLEMENTED";
        case HTTP_VERSION_NOT_SUPPORTED:        return "HTTP VERSION NOT SUPPORTED";
        case FORBIDDEN:                         return "FORBIDDEN";
        default:                               	return "UNKNOWN STATUS CODE";
    }
}

static std::string get_content_type(std::string extension)
{
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

static std::string get_page_content(std::ifstream &file)
{
    std::ostringstream file_buffer;

    file_buffer << file.rdbuf();

    return (file_buffer.str());
}


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

//? TEMPORARY
Get::Get(const int client_fd, Request &request, Server &server, size_t status_code) : _client_fd(client_fd), _request(request), _server(server), _fd(0) {
    if (request.getExtension() == "listing") {
        std::cout << "Need to do the listing" << std::endl;
        std::cout << request.getPath() << std::endl;
    }
    if (request.getExtension() == "directory") {
        std::cout << R << "FORBIDDEN" << C << std::endl;
        generate_response(403, request, client_fd);
        _fd = send(client_fd, _content.c_str(), _content.size(), 0);
        if (_fd < 0) {
            throw std::runtime_error("Send failed");
        }
    }
    else {
        generate_response(status_code, request, client_fd);
        _fd = send(client_fd, _content.c_str(), _content.size(), 0);
        if (_fd < 0) {
            throw std::runtime_error("Send failed");
        }
    }
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Get::~Get() {
	if (_fd >= 0)
		close(_fd);
}

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


void Get::generate_response(size_t status_code, Request &request, const int &client_fd)
{
    std::ifstream file(request.getPath().c_str());
	std::string content;

	if (!file.is_open() || !file.good())
	{
		status_code = ERROR_NOT_FOUND;
        file.open(std::string(request.getLocation()->getRoot() + '/' + request.getLocation()->getErrorPage()).c_str());
		if (!file.is_open() || !file.good())
			content = "<h1>default: 404</h1>";
	}
	else
        content = get_page_content(file);

	const std::string status_message = get_status_message(status_code);
	const std::string content_type = get_content_type(request.getExtension());


    _content = ft_to_string(HTML_VERSION) + " " + ft_to_string(status_code) + " " + status_message + "\n" \
								+ "Content-Type: " + content_type + "\n" + "Content-Length: " + ft_to_string(content.size()) \
								+ "\n\n" + content + "\0";
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */