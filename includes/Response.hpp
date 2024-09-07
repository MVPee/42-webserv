#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "macro.hpp"

class Request;
class Server;

class Response {
	private:
		size_t			&_status_code;
		Request			&_request;

		const std::string		generate_response(const std::string &page_content) const;
	public:
		Response(int &client_fd, Request &request, Server &server, int &sd);
		~Response();
};

std::ostream &			operator<<( std::ostream & o, Response const & i );

#endif /* ******************************************************** RESPONSE_H */