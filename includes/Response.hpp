#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "macro.hpp"

class Request;
class Server;
class Cookie;
class Client;

class Response {
	private:
		size_t			&_status_code;
		Request			&_request;
		Server			&_server;
		Client 			&_client;
		Cookie			*_cookie;
		std::string		_response;

		const std::string		generate_response(const std::string &page_content) const;
		std::string 			setCookie(const std::string &request) const;
	public:
		Response(Client &client);
		~Response();
		const std::string &getResponse(void) { return _response; }
};

#endif /* ******************************************************** RESPONSE_H */