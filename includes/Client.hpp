#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "macro.hpp"


#define HEADER_DELIMITER "\r\n\r\n"

enum ClientState {
    ReceivingHeader,
    HandlingBody,
    Completed,
    Error
};

class Post;

class Client
{
	private:
		int				_client_fd;
		std::string 	_header;
		std::string 	_body;
		time_t			_connection_time;
		ClientState		_state;
		Request			*_request;
		Post			*_post;
		
		Server			&_server;

	public:

		Client(Server &s, int fd);
		~Client();

		const int &getFd(void) const { return (_client_fd); }
		const std::string &getHeader(void) const { return (_header); }
		const std::string &getBody(void) const { return (_body); }
		const time_t &getConnectionTime(void) const { return (_connection_time); }
		void handle_client( void );
		void receive_content( void );

		void setFd(int fd) { _client_fd = fd; }
		void setHeader(int header) { _header = header; }
		void setBody(int body) { _body = body; }
		void setConnectionTime(time_t connection_time) { _connection_time = connection_time; }


};

#endif /* ********************************************************** CLIENT_H */