#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "macro.hpp"

class Request;
class Server;

class Response {
	private:
		int _fd;
		std::string _content;
		size_t		_content_size;
		void getContent(Request &request, Server &server);
	public:
		Response(int &client_fd, Request &request, Server &server);
		~Response();
};

std::ostream &			operator<<( std::ostream & o, Response const & i );

#endif /* ******************************************************** RESPONSE_H */