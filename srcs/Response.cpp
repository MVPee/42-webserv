#include "../includes/Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(int &client_fd, Request &request, Server &server) {
    
    if (request.getMethod() == POST) {
        std::cout << R "POST" C << std::endl;
		Post(client_fd, request, server);
    }
    if (request.getLocation()->getMethods(GET)) {
        std::cout << R "GET" C << std::endl;
	    Get(client_fd, request, server);
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