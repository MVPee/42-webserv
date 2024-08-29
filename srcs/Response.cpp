#include "../includes/Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

// image/png, image/x-icon, image/jpeg, ...
// text/css, text/html
// application/javascript
//code error
//php html ico



// static std::string getContent(std::string request)
// {
// 	std::size_t start;
// 	std::string extension;

// 	start = request.find('.');
// 	if (start == std)
// }

// "./html" | 


static std::string getExtension(std::string file) {
	size_t start = file.find('.', 1);
	if (start == std::string::npos)
		return ("none");

	start += 1;
	std::string extension = file.substr(start, file.size() - start);
	// std::cout << R << extension << C << std::endl; //* DEBUG
	if (extension == "php") return "text/php";
	else if (extension == "ico") return "image/x-icon";
	else return "text/html";
}

static std::string getFile(std::string page, Server &server) {
	std::string extension = getExtension(page);
	if (page == (server.getRoot() + "/"))
		return (page + server.getIndex());
	if (extension == "none")
		return (page + ".html");
	return (page);
}

static std::string getContent(std::string page) {
	std::ostringstream html;
	struct stat stat_buf;
	// std::cout << Y << page << C << std::endl; //* DEBUG
	std::ifstream file(page.c_str(), std::ifstream::binary | std::ifstream::in);
	if (!file.is_open() || !file.good())
		return ("HTTP/1.1 404 NOT FOUND\nContent-Type:text/html\nContent-Length: 12\n\n<h1>404</h1>");
	if (stat(page.c_str(), &stat_buf)) throw std::runtime_error("Error while retrieving request data");

	std::ostringstream string_converter;
	string_converter << stat_buf.st_size;

	// std::cout << B << string_converter.str() << C << std::endl; //* DEBUG
	std::ostringstream str1;
	str1 << file.rdbuf();
	std::string content = str1.str();
	html << "HTTP/1.1 200 Ok\nContent-Type:" + getExtension(page) + "\nContent-Length: " << content.size() << "\n\n" << content;
	return (html.str());
}

Response::Response(int &client_fd, Request &request, Server &server) {
	std::string file = getFile(request.getPath(), server);
	// std::cout << file << std::endl; //* DEBUG
	std::string content = getContent(file);
    _fd = send(client_fd, content.c_str(), content.size(), 0);
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

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */