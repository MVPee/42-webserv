#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "macro.hpp"



class Post;

class Client
{
	private:
		int				_client_fd;
		std::string 	_header;
		std::string 	_body;
		std::string 	_response;
		time_t			_connection_time;
		ClientState		_state;
		Request			*_request;
		Post			*_post;
		
		Server			&_server;
		
		void receive_request_content ( void );
	public:

		Client(Server &s, int fd);
		~Client();

		void request( void );
		void response( void );
		bool checkTimeOut(void);
		void clear(void);

		ClientState		getState(void) {return(_state);}
		Request		&getRequest( void ) {return(*_request);}
		Server 		&getServer( void ) {return(_server);}

		const int &getFd(void) const { return _client_fd; }
		const std::string &getHeader(void) const { return _header; }
		const std::string &getBody(void) const { return _body; }
		const time_t &getConnectionTime(void) const { return _connection_time; }

		void 		setState(ClientState new_state) {_state = new_state;}
		void 		setFd(int fd) { _client_fd = fd; }
		void 		setHeader(std::string header) { _header = header; }
		void 		setBody(std::string body) { _body = body; }
		void 		setConnectionTime(time_t connection_time) { _connection_time = connection_time; }


};

#endif /* ********************************************************** CLIENT_H */