#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "macro.hpp"

class Request {
	private:
		int 	_fd;
		char 	_buffer[1024];
	public:
		Request(int &client_fd);
		~Request();
		
		char	*getBuffer(void);
		int		&getFd(void);
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */