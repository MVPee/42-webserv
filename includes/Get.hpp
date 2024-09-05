#ifndef GET_HPP
# define GET_HPP

# include "macro.hpp"

class Get {
	private:
		const int		_client_fd;
		const Request	&_request;
		const Server	&_server;
		int				status_code;
		int				_fd;
		std::string		_content;
		size_t			_content_size;

		void getContent(Request &request, Server &server);
	public:
		Get(const int client_fd, Request &request, Server &server);
		~Get();
};

std::ostream &			operator<<( std::ostream & o, Get const & i );

#endif /* ************************************************************* GET_H */