#ifndef POST_HPP
# define POST_HPP

# include "../macro.hpp"


class Post {

	private:
		const int 		_client_fd;
		const Request 	&_request;
		const Server 	&_server;
		ssize_t			_body_size;

		std::string		_remaining_content;
		std::string		_boundary;
		std::string		_header;
		size_t			&_status_code;
		ClientState 		_state;

		void		handle_post_request(Location *location, std::string &new_content);
		void 		output_Content_Body(std::ofstream &output_file, std::string &filename, std::string &new_content);
		std::string	receive_content_header(void);
		void		throw_and_set_status(const size_t status_code, std::string message);

	public:

		ClientState	get_state( void ) {return (_state);};
		void		decide_action ( std::string &new_content);
		Post(const int client_fd, Request &request, Server &server);
		~Post();
};

std::ostream &			operator<<( std::ostream & o, Post const & i );

#endif /* ************************************************************ POST_H */