#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "macro.hpp"

class Request;
class Server;

class Response {
	private:
		int _fd;
	public:
		Response(int &client_fd, Request &request, Server &server);
		~Response();
};

std::ostream &			operator<<( std::ostream & o, Response const & i );

#endif /* ******************************************************** RESPONSE_H */