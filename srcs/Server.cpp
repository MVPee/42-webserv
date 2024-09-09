#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(const std::string config_text) :
	name("default_server"), 
	port(80), 
	body(100) {
	std::map<std::string, std::string> map;

	//! Temporary
	Config config(config_text);

	//! Need to use and parse nicely in Request and Response
	name = config.getServerName();
	port = config.getPort();
	body = config.getBody();
	_locations = config.getLocations();
	//std::cout << _locations << std::endl; //* DEBUG
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server() {
	if (_fd_socket)
		close(_fd_socket);
	while(!_locations.empty() && _locations.size() > 0)
	{
		delete _locations.back();
		_locations.pop_back();
	}
	for (size_t i = 0; i < MAX_CLIENT; i++) {
		delete _clients[i];
	}
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Server const & i ) {
	o << M << "\n----Server----\n";
	o << "|Name: " << i.getName() << "\n";
	o << "|Port: " << i.getPort() << "\n";
	o << "|Body: " << i.getBody() << "\n";
	o << "-----End-----\n" << C;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Server::mySocket(void) {
	// memset(_client_socket, 0, MAX_CLIENT * sizeof(int));
	for (int i = 0; i < MAX_CLIENT; i++) {
        _clients[i] = new Client(*this, 0);
    }

	if ((_fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Socked failed");

	this->sock_address.sin_family = AF_INET;
    this->sock_address.sin_port = htons(this->port);
    this->sock_address.sin_addr.s_addr = INADDR_ANY;
}

void Server::myBind(void) {
	int opt = 1;
	if (setsockopt(_fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw std::runtime_error("setsockopt failed");
	}
	if (bind(_fd_socket, (sockaddr *) &this->sock_address, sizeof(this->sock_address)) < 0)
		throw std::runtime_error("Bind failed");
}

void Server::myListen(void) {
	if (listen(_fd_socket, MAX_CLIENT) < 0)
		throw std::runtime_error("Listen failed");

	std::ostringstream ss;
	ss << "\n*** Listening on ADDRESS: " 
		<< inet_ntoa(sock_address.sin_addr) 
		<< " PORT: " << ntohs(sock_address.sin_port) 
		<< " ***\n\n";
	std::cout << ss.str() << std::endl; //* DEBUG
}

void Server::process(void) {
	FD_ZERO(&_readfds);
    FD_ZERO(&_writefds);

    FD_SET(_fd_socket, &_readfds);
    FD_SET(_fd_socket, &_writefds);
	_max_sd = _fd_socket;

	for (int i = 0; i < MAX_CLIENT; i++) {
		_sd = _clients[i]->getFd();
		if (_sd > 0) {
			FD_SET(_sd, &_readfds);
			FD_SET(_sd, &_writefds);
			if (_sd > _max_sd)
				_max_sd = _sd;
		}
	}

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	if (select(_max_sd + 1, &_readfds, &_writefds, NULL, &timeout) < 0)
		std::cerr << "Erreur avec select(), errno: " << strerror(errno) << std::endl;

	if (stopRequested) return;

	if (FD_ISSET(_fd_socket, &_readfds)) {
		int addrlen = sizeof(this->sock_address);
		if ((_new_socket = accept(_fd_socket, (sockaddr *) &this->sock_address, (socklen_t *)&addrlen)) < 0)
			throw std::runtime_error("Accept failed");
		for (int i = 0; i < MAX_CLIENT; i++) {
			if (_clients[i]->getFd() == 0) {
				_clients[i]->setFd(_new_socket);
				_clients[i]->setConnectionTime(time(NULL));
				break;
			}
		}
	}

	if (stopRequested) return;

	for (int i = 0; i < MAX_CLIENT; i++) {
        _sd = _clients[i]->getFd();
		// if (FD_ISSET(_sd, &_readfds) || FD_ISSET(_sd, &_writefds)) {
		// 	time_t current_time = time(NULL);
		// 	if (difftime(current_time, _clients[i]->getConnectionTime()) > TIME_OUT) {
		// 		std::cout << _sd << ": time out..." << std::endl;
		// 		std::string time_out = "HTTP/1.1 408 Request Timeout\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\n<h1>Time out 408</h1>";
		// 		send(_sd, time_out.c_str(), time_out.size(), 0);
		// 		_clients[i]->setFd(0);
		// 		_clients[i]->setHeader("");
		// 		_clients[i]->setBody("");
		// 		_clients[i]->setConnectionTime(-1);
		// 		close(_sd);
		// 	}
		// }

		if (FD_ISSET(_sd, &_readfds)) {
			_clients[i]->receive_content();
			std::cout << _clients[i]->getHeader() << std::endl;
		}
		if (FD_ISSET(_sd, &_writefds)) {
			_clients[i]->handle_client();
			if (_clients[i]->getState() == Completed)
				_clients[i]->clear();
		}

		if (stopRequested) return;
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string &Server::getName(void) const { return (this->name); }
const unsigned int &Server::getPort(void) const { return (this->port); } 
const unsigned int &Server::getBody(void) const { return (this->body); }
const std::vector<Location *> &Server::getLocations(void) const { return (this->_locations); }

/* ************************************************************************** */