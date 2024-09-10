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
		int _max_sd, _new_socket;

		char _buffer[2048];

		std::string name;
		unsigned int port;
		unsigned int body;
		std::vector<Location *> 	_locations;

		sockaddr_in sock_address;
		int _fd_socket;

		Request		*_request;
		Response	*_response;

		void handleRequest(int &_sd);
	public:
		Server(const std::string config_text);
		~Server();

		const std::string &getName(void) const;
		const unsigned int &getPort(void) const;
		const unsigned int &getBody(void) const;
		const std::vector<Location *> &getLocations(void) const;

		void mySocket(void);
		void myBind(void);
		void myListen(void);
		void process(void);
};

std::ostream &operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */