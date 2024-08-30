#include "../includes/Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

//? Temporary function
std::string get_content_type(std::string extension)
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

Response::Response(int &client_fd, Request &request, Server &server) {
    
    if (request.getMethod() == POST)
	    std::cout << R "POST" C << std::endl;
    if (request.getMethod() == GET) {
	    getContent(request, server);
        _fd = send(client_fd, _content.c_str(), _content_size, 0);
        if (_fd < 0) {
            throw std::runtime_error("Send failed");
        }
    }
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {
    if (_fd >= 0)
	    close(_fd);
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

static void putContent(const std::ifstream &file, std::string &_content) {
    std::ostringstream temp;

    temp << file.rdbuf();
    std::string content = temp.str();
    std::ostringstream sizeStream;
    sizeStream << content.size();
    _content += sizeStream.str() + "\n\n" + content;
}

void Response::getContent(Request &request, Server &server) {
    std::ifstream file(request.getPath().c_str(), std::ios::binary);

    _content += "HTTP/1.1 ";
    if (!file.is_open() || !file.good()) {
        file.open(std::string(server.getRoot() + '/' + server.getError()).c_str());
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