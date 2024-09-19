#include "../includes/Server.hpp"

/*
** ------------------------------- STATIC ------------------------------------
*/

std::string getCurrentTime() {
    std::time_t now = std::time(0);
    std::tm* time_info = std::localtime(&now);
    
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", time_info);
    return std::string(buffer);
}

void Server::message(const std::string &message, const char *color) const {
	std::ostringstream ss;
	ss 	<< M << "[" << getCurrentTime() << "] " << C 
		<< "[" << BOLD << _name << C << "] "
		<< "[" << ITALIC << ntohs(_sock_address.sin_port) << C << "]"
		<< "\t\t" << color << message << C;
	std::cout << ss.str() << std::endl; //* LOG
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(const std::string config_text) :
	_name("default"), 
	_port(80), 
	_bodySize(100),
	_socket(0) {

	Config config(config_text);
	_name = config.getServerName();
	_port = config.getPort();
	_bodySize = config.getBody();
	_locations = config.getLocations();
	_address = config.getAddress();
	//std::cout << _locations << std::endl; //* DEBUG
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server() {
	if (_socket)
		close(_socket);
	while(!_locations.empty() && _locations.size() > 0) {
		delete _locations.back();
		_locations.pop_back();
	}
	for (size_t i = 0; i < MAX_CLIENT; i++)
		delete _clients[i];
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
	for (int i = 0; i < MAX_CLIENT; i++)
        _clients[i] = new Client(*this, 0);

	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Socked failed");
}

void Server::myBind(void) {
	int opt = 1;

	_sock_address.sin_family = AF_INET;
    _sock_address.sin_port = htons(_port);
    _sock_address.sin_addr.s_addr = inet_addr(_address.c_str());
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Setsockopt failed");
	if (bind(_socket, (sockaddr *) &_sock_address, sizeof(_sock_address)) < 0)
		throw std::runtime_error("Bind failed, address or port is probably invalid");
}

void Server::myListen(void) {
	if (listen(_socket, MAX_CLIENT) < 0)
		throw std::runtime_error("Listen failed");
	message("Running", G);
}

void Server::process(void) {
	FD_ZERO(&_readfds);
    FD_ZERO(&_writefds);

    FD_SET(_socket, &_readfds);
    FD_SET(_socket, &_writefds);
	int max_sd = _socket;

	for (int i = 0; i < MAX_CLIENT; i++) {
		if (_clients[i]->getFd() > 0) {
			FD_SET(_clients[i]->getFd(), &_readfds);
			FD_SET(_clients[i]->getFd(), &_writefds);
			if (_clients[i]->getFd() > max_sd)
				max_sd = _clients[i]->getFd();
		}
	}

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	if (select(max_sd + 1, &_readfds, &_writefds, NULL, &timeout) < 0)
		throw std::runtime_error("Select failed");

	if (FD_ISSET(_socket, &_readfds)) {
		int addrlen = sizeof(_sock_address);
		int new_socket;
		if ((new_socket = accept(_socket, (sockaddr *) &_sock_address, (socklen_t *)&addrlen)) < 0)
			throw std::runtime_error("Accept failed");
		for (int i = 0; i < MAX_CLIENT; i++) {
			if (_clients[i]->getFd() == 0) {
				_clients[i]->setFd(new_socket);
				_clients[i]->setConnectionTime(time(NULL));
				break;
			}
		}
	}

	if (stopRequested) {
		message("Stopping", R);
		return ;
	}

	for (int i = 0; i < MAX_CLIENT; i++) {
		if (FD_ISSET(_clients[i]->getFd(), &_readfds))
			_clients[i]->request();
		if (FD_ISSET(_clients[i]->getFd(), &_writefds))
			_clients[i]->response();
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string &Server::getName(void) const { return _name; }
const unsigned int &Server::getPort(void) const { return _port; } 
const long &Server::getBody(void) const { return _bodySize; }
const std::vector<Location *> &Server::getLocations(void) const { return _locations; }

/* ************************************************************************** */