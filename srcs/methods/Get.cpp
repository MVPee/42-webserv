#include "../../includes/methods/Get.hpp"


const char* get_status_message(const size_t status_code) {
    switch (status_code) {
        case OK:                                return "OK";
        case REDIRECTION_PERMANENTLY:           return "Moved Permanently";
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

Get::Get(const int client_fd, Request &request, Server &server) : 
_client_fd(client_fd), 
_request(request), 
_server(server), 
_fd(0),
_status_code(request.get_status_code()) {
    if (!request.isAccepted() || request.getExtension() == "directory")
        _status_code = FORBIDDEN;
    else if (request.getExtension() == "redirection")
        generate_redirection(request.getLocation()->getRedirection());
    else if (request.getExtension() == "listing")
        generate_listing();
    if (_content.empty())
        generate_response();
    _fd = send(client_fd, _content.c_str(), _content.size(), 0);
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

void Get::generate_redirection(std::string redirection) {
    std::string content = ft_to_string(get_status_message(_status_code)) + ". Redirecting to " + redirection;
        _content =  ft_to_string(HTML_VERSION) + " " + ft_to_string(_status_code) + "\n" \
                    + "Location: " + redirection + '\n' \
                    + "Content-Length: " + ft_to_string(content.size()) + '\n' \
                    + "Connection: close" + "\n\n" \
                    + content + '\0';
}

void Get::generate_listing( void ) {
    struct dirent* entry;
    std::string content;
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
    std::cout << R << absolutePath << C << std::endl;
    while ((entry = readdir(dir)) != NULL) {
        dirname = entry->d_name;
        if (entry->d_type == DT_DIR)
            content += "<a href=\"" + absolutePath + dirname + "\">" + dirname + "/" + "</a><br>" + '\n';
        if (entry->d_type == DT_REG)
            content += "<a href=\"" + absolutePath + dirname + "\">" + dirname + "</a><br>" + '\n';
    }
    _content = ft_to_string(HTML_VERSION) + " " + ft_to_string(_status_code) + " " + get_status_message(_status_code) + "\n" \
                            + "Content-Type: text/html\n" + "Content-Length: " + ft_to_string(content.size()) \
                            + "\n\n" + content + "\0";
    std::cout << B << _content << C << std::endl; //? DEBUG
    closedir(dir);
}

void Get::generate_response()
{
    std::ifstream file(_request.getPath().c_str());
	std::string page_content;
    std::string content_type;

    //? TEMPORAIRE
	if (!file.is_open() || !file.good() || _status_code >= 400 || _request.getMethod() == DELETE) {
        content_type = "text/html";
        if ((!file.is_open() || !file.good()) && _status_code < 400 && _request.getMethod() != DELETE)
		    _status_code = ERROR_NOT_FOUND;
        if (!_request.getLocation()->getErrorPage(_status_code).empty()) {
            if (file.is_open())
                file.close();
            file.open(ft_to_string(_request.getLocation()->getRoot() + '/' + _request.getLocation()->getErrorPage(_status_code)).c_str());
            if (file.is_open() && file.good()) {
                page_content = ft_to_string(file.rdbuf());
            }
        }
        if (page_content.empty())
		    page_content = "<h1>default: " + ft_to_string(_status_code) + " " + get_status_message(_status_code) + "</h1>";
	}
	else
        page_content = get_page_content(file);

	const std::string status_message = get_status_message(_status_code);
    if (content_type.empty())
        content_type = get_content_type(_request.getExtension());


    _content = ft_to_string(HTML_VERSION) + " " + ft_to_string(_status_code) + " " + status_message + "\n" \
								+ "Content-Type: " + content_type + "\n" + "Content-Length: " + ft_to_string(page_content.size()) \
								+ "Connection: close" \
								+ "\n\n" + page_content + "\0";
    if (file.is_open())
        file.close();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */