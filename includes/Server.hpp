#ifndef SERVER_HPP
# define SERVER_HPP

# include "macro.hpp"

class Request;
class Response;
class Location;
class Client;

class Server {
	private:
		std::map<int, Client *> _clients;
		fd_set _readfds, _writefds;

		std::string 				_name;
		std::string					_address;
		unsigned int 				_port;
		long		 				_bodySize;
		std::vector<Location *> 	_locations;

		sockaddr_in 				_sock_address;
		int							_socket;
	public:
		Server(const std::string config_text);
		~Server();

		const std::string	&getName(void) const;
		const unsigned int	&getPort(void) const;
		const long			&getBody(void) const;
		const std::vector<Location *> &getLocations(void) const;

		void mySocket(void);
		void myBind(void);
		void myListen(void);
		void process(void);
};

std::ostream &operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */