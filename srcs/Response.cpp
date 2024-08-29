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

	std::cout << request.getPath() << std::endl;
	std::cout << request.getExtension() << std::endl;

	getContent(request, server);
    _fd = send(client_fd, _content.c_str(), _content_size, 0);
    if (_fd < 0) {
        throw std::runtime_error("Send failed");
	}
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {
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

void Response::getContent(Request &request, Server &server) {
    std::ostringstream temp;
    std::ifstream file(request.getPath().c_str());

    _content += "HTTP/1.1 ";
    if (!file.is_open() || !file.good()) {
        file.open(std::string(server.getRoot() + '/' + server.getError()).c_str());
        _content += "404 NOT FOUND\nContent-Type: text/html\nContent-Length: ";
        if (!file.is_open() || !file.good()) {
            _content += "21\n\n<h1>default: 404</h1>";
        } else {
            temp << file.rdbuf();
            std::string content = temp.str();
            std::ostringstream sizeStream;
            sizeStream << content.size();
            _content += sizeStream.str() + "\n\n" + content;
        }
    } else {
        _content += "200 OK\nContent-Type: " + get_content_type(request.getExtension()) + "\nContent-Length: ";
        temp << file.rdbuf();
        std::string content = temp.str();
        std::ostringstream sizeStream;
        sizeStream << content.size();
        _content += sizeStream.str() + "\n\n" + content;
    }
	_content_size = _content.size();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */