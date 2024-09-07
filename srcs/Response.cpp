#include "../includes/Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(int &client_fd, Request &request, Server &server): _status_code(request.get_status_code()) {
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
	std::cout << B << _status_code << C << std::endl;
	Get(client_fd, request, server);
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