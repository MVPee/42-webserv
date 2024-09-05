#include "../../includes/methods/Get.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Get::Get(const int client_fd, Request &request, Server &server) : _client_fd(client_fd), _request(request), _server(server), _fd(0) {
    if (request.getExtension() == "listing")
        std::cout << "Need to do the listing" << std::endl;
	getContent(request, server);
	_fd = send(client_fd, _content.c_str(), _content_size, 0);
	if (_fd < 0) {
		throw std::runtime_error("Send failed");
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

static void putContent(const std::ifstream &file, std::string &_content) {
    std::ostringstream temp;

    temp << file.rdbuf();
    std::string content = temp.str();
    std::ostringstream sizeStream;
    sizeStream << content.size();
    _content += sizeStream.str() + "\n\n" + content;
}

void Get::getContent(Request &request, Server &server) {
    std::ifstream file(request.getPath().c_str(), std::ios::binary);

    _content += "HTTP/1.1 ";
    if (!file.is_open() || !file.good()) {
        file.open(std::string(request.getLocation()->getRoot() + '/' + request.getLocation()->getErrorPage()).c_str());
        _content += "404 NOT FOUND\nContent-Type: text/html\nContent-Length: ";
        if (!file.is_open() || !file.good())
            _content += "21\n\n<h1>default: 404</h1>";
        else
            putContent(file, _content);
    }
    else {
        _content += "200 OK\nContent-Type: " + get_content_type(request.getExtension()) + "\nContent-Length: ";
        putContent(file, _content);
    }
    if (file.is_open())
        file.close();
	_content_size = _content.size();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */