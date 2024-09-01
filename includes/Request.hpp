#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "macro.hpp"
class Server;

class Request {
	private:
		short	_method;
		bool	_accept;

		std::string _header;
		std::string	_extension;
		std::string _path;
		std::size_t _content_length;

		void parse_request(Server &s);
		void parse_extension( void );
		void resolvePath(Server &s);
	public:
		Request(int &client_fd, Server &s);
		~Request();

		const std::size_t	&getContent_Length(void) const;
		const std::string	&getHeader(void) const;
		const short			&getMethod(void) const;
		const std::string	&getExtension(void) const;
		const std::string	&getPath(void) const;
		const bool			&isAccepted(void) const;
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */