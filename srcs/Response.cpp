#include "../includes/Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(int &client_fd, Request &request, Server &server) {
	size_t status_code = 200;
    
    if (request.getMethod() == POST) {
		Post Form(client_fd, request, server);
		status_code = Form.get_status_code();
    }
    if (request.getLocation()->getMethods(GET)) {
	    Get(client_fd, request, server, status_code);
    }
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