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

Response::Response(int &client_fd, Request &request, Server &server, int &sd):
_status_code(request.get_status_code()),
_request(request),
_cookie(0) {
	std::string response_header;

	if (request.getMethod() == DELETE) {
		if (request.isAccepted()) {
			if (access(request.getPath().c_str(), F_OK) != 0)
				_status_code = ERROR_NOT_FOUND;
			else if (remove(request.getPath().c_str()))
				_status_code = ERROR_INTERNAL_SERVER;
			else 
				_status_code = NO_CONTENT;
		}
	}
	else if (request.getExtension() == "cgi")
	{
		Cgi cgi(_request);
		std::cout << cgi << std::endl;
		response_header = cgi.getResponseContent();
	}
	else if (request.getMethod() == GET) {
		response_header = Get (request, server).get_content();
		_cookie = new Cookie(_request.getHttpRequest(), response_header);
	}

	_response = generate_response(response_header);
}

std::string Response::setCookie(const std::string &request) const {
    std::string result("");
    if (request.find(" /cookie ") != std::string::npos ||
        request.find(" /cookie/ ") != std::string::npos ||
        request.find(" /cookie/cookie ") != std::string::npos ||
        request.find(" /cookie/cookie.html ") != std::string::npos) {
        result = "Set-Cookie: id=" + ft_to_string(_cookie->getId()) + "; Path=/cookie;\r\n";
    }
    return (result);
}

const std::string Response::generate_response(const std::string &page_content) const {
    std::string content_type = _status_code < 400 ? get_content_type(_request.getExtension()) : "text/html";
    const std::string status_message = get_status_message(_status_code);

    std::string response = ft_to_string(HTML_VERSION) + " " + ft_to_string(_status_code) + " " + status_message + "\r\n" \
                            + "Content-Type: " + content_type + "\r\n" \
                            + "Content-Length: " + ft_to_string(page_content.size()) + "\r\n"\
                            + setCookie(_request.getHttpRequest())
                            + "Connection: close" \
                            + ((_request.getExtension() == "redirection") ? ("\r\nLocation: " + _request.getLocation()->getRedirection() + "\r\n") : ("")) \
                            + "\r\n\r\n" + page_content + '\0';
    return response;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {
	if (_cookie)
		delete _cookie;
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