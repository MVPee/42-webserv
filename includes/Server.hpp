#ifndef SERVER_HPP
# define SERVER_HPP

# include "macro.hpp"

class Request;
class Response;
class Location;

class Server {
	private:
		int _client_socket[MAX_CLIENT];
		std::map<int, std::string> _responses;
		std::map<int, std::string> _requests;
		//time_t _connection_times[MAX_CLIENT];
		fd_set _readfds, _writefds;
		int _max_sd, _sd, _new_socket;

		std::string name;
		unsigned int port;
		unsigned int body;
		std::vector<Location *> 	_locations;

		sockaddr_in sock_address;
		int _fd_socket;

		Request		*_request;
		Response	*_response;
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