#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "macro.hpp"
class Server;

class Request {
	private:
		int 	_fd;
		char 	_buffer[1024];
		short	_method;
		std::string	_extension;
		std::string _path;

		void parse_request(Server &s);
		void getExtension( void );
	public:
		Request(int &client_fd, Server &s);
		~Request();
		
		const char			*getBuffer(void) const;
		const int			&getFd(void) const;
		const short			&getMethod(void) const;
		const std::string	&getExtension(void) const;
		const std::string	&getPath(void) const;
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */