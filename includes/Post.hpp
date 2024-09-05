#ifndef POST_HPP
# define POST_HPP

# include "macro.hpp"

class Post {

	private:
		const int _client_fd;
		const Request &_request;
		const Server &_server;
		ssize_t		_body_size;

		std::string	_remaining_content;
		std::string _boundary;
		int			status_code;

		void		handle_post_request(void);
		void 		output_Content_Body(std::ofstream &output_file);
		std::string	receive_content_header(void);
	public:

		Post(const int client_fd, Request &request, Server &server);
		~Post();
};

std::ostream &			operator<<( std::ostream & o, Post const & i );

#endif /* ************************************************************ POST_H */