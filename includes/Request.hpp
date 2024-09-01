#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "macro.hpp"
class Server;

class Request {
	private:
		short	_method;
		bool	_accept;
		char	_content[4096];
		std::string _httpRequest;
		std::string	_extension;
		std::string _path;

		void parse_request(Server &s);
		void parse_extension( void );
		void resolvePath(Server &s);
	public:
		Request(int &client_fd, Server &s);
		~Request();
		
		const char			*getContent(void) const;
		const std::string	&getHttpRequest(void) const;
		const short			&getMethod(void) const;
		const std::string	&getExtension(void) const;
		const std::string	&getPath(void) const;
		const bool			&isAccepted(void) const;
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */