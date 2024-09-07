#include "../includes/Response.hpp"

/*
** ------------------------------- STATIC --------------------------------
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

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(int &client_fd, Request &request, Server &server):
 _status_code(request.get_status_code()),
 _request(request) {
    if (request.getMethod() == POST && request.isAccepted()) {
		Post Form(client_fd, request, server);
    }
	else if (request.getMethod() == DELETE  && request.isAccepted())
	{
		if (access(request.getPath().c_str(), F_OK) != 0)
			_status_code = ERROR_NOT_FOUND;
		else if (remove(request.getPath().c_str()))
			_status_code = ERROR_INTERNAL_SERVER;
	}
	Get get(client_fd, request, server);

	const std::string response = generate_response(get.get_content());
    int fd = send(client_fd, response.c_str(), response.size(), 0);
    if (fd < 0) {
        throw std::runtime_error("Send failed"); //? Catch ?
    }


}


const std::string Response::generate_response(const std::string &page_content) const
{
    std::string content_type = _status_code < 400 ? get_content_type(_request.getExtension()) : "text/html";
	const std::string status_message = get_status_message(_status_code);

	std::string response = ft_to_string(HTML_VERSION) + " " + ft_to_string(_status_code) + " " + status_message + '\n' \
								+ "Content-Type: " + content_type + '\n' \
								+ "Content-Length: " + ft_to_string(page_content.size()) + '\n'\
								+ "Connection: close" \
								+ ((_request.getExtension() == "redirection") ? ("\nLocation: " + _request.getLocation()->getRedirection() + '\n') : ("")) \
								+ "\n\n" + page_content + '\0';
	return (response);
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

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */