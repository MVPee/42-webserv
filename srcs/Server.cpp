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
        _client_socket[i] = 0;
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
		_sd = _client_socket[i];
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
	if ((select(_max_sd + 1, &_readfds, &_writefds, NULL, &timeout) < 0) && (errno != EINTR)) {
		std::cerr << "Erreur avec select(), errno: " << strerror(errno) << std::endl;
	}

	if (stopRequested) return;

	if (FD_ISSET(_fd_socket, &_readfds)) {
		int addrlen = sizeof(this->sock_address);
		if ((_new_socket = accept(_fd_socket, (sockaddr *) &this->sock_address, (socklen_t *)&addrlen)) < 0)
			throw std::runtime_error("Accept failed");
		for (int i = 0; i < MAX_CLIENT; i++) {
			if (_client_socket[i] == 0) {
				_client_socket[i] = _new_socket;
				_connection_times[i] = time(NULL);
				break;
			}
		}
	}

	if (stopRequested) return;

	int bytes_received;
	for (int i = 0; i < MAX_CLIENT; i++) {
        _sd = _client_socket[i];
		if (FD_ISSET(_sd, &_readfds) || FD_ISSET(_sd, &_writefds)) {
			time_t current_time = time(NULL);
			if (_connection_times.find(_sd) == _connection_times.end())
				_connection_times[_sd] = time(NULL);
			else if (difftime(current_time, _connection_times[_sd]) > TIME_OUT) {
				std::cout << _sd << ": time out..." << std::endl;
				std::string time_out = "HTTP/1.1 408 Request Timeout\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\n<h1>Time out 408</h1>";
				send(_sd, time_out.c_str(), time_out.size(), 0);
				close(_sd);
				_client_socket[i] = 0;
				_header.erase(_sd);
				_body.erase(_sd);
				_connection_times.erase(_sd);
			}
		}

		if (FD_ISSET(_sd, &_readfds)) {
			if (_header[_sd].find("\r\n\r\n") == std::string::npos)
				bytes_received = recv(_sd, _buffer, 1, 0);
			if (bytes_received > 0 || _header[_sd].find("\r\n\r\n") != std::string::npos) {
				_buffer[bytes_received] = '\0';
				_header[_sd] += std::string(_buffer);
				if (_header[_sd].find("\r\n\r\n") != std::string::npos) {
					if (_header[_sd].find("GET") != std::string::npos || _header[_sd].find("DELETE") != std::string::npos) {
						std::string full_request = _header[_sd];
						//!	REQUEST		/
						//! std::cout << Y << "Socket " << _sd << " :\n" << full_request << C << std::endl;
						_response = new Response(*this, full_request);
						std::string httpResponse = _response->getHttpResponse();
						//!	RESPONSE	/
						//! std::cout << B << httpResponse << C << std::endl;
						_responses[_sd] = httpResponse;
						delete _response;
						_header.erase(_sd);
						_connection_times.erase(_sd);
					}
					else if (_header[_sd].find("POST") != std::string::npos) {
						std::string::size_type pos = _header[_sd].find("Content-Length");
						if (pos != std::string::npos) {
							std::string::size_type end_pos = _header[_sd].find("\r\n", pos);
							std::string length = _header[_sd].substr(pos + 16, end_pos - pos - 16);
							if (atoi(length.c_str()) != _body[_sd].size()) {
								bytes_received = recv(_sd, _buffer, sizeof(char) * sizeof(_buffer), 0);
								_buffer[bytes_received] = '\0';
								_body[_sd] += std::string(_buffer);
								if (atoi(length.c_str()) == _body[_sd].size()) {
									std::cout << "here" << std::endl;
									std::cout << _body[_sd] << std::endl;
									std::string no_content = "HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n";
									send(_sd, no_content.c_str(), no_content.size(), 0);
									_client_socket[i] = 0;
									_header.erase(_sd);
									_body.erase(_sd);
									_connection_times.erase(_sd);
									close(_sd);
								}
							}
						}
					}
				}
			}
			else {
				if (bytes_received == 0)
					std::cout << "Client close connexion..." << std::endl;
				_client_socket[i] = 0;
				_header.erase(_sd);
				_body.erase(_sd);
				_connection_times.erase(_sd);
				close(_sd);
			}
		}
		if (FD_ISSET(_sd, &_writefds)) {
			if (_responses.find(_sd) != _responses.end()) {
				std::string &response_data = _responses[_sd];
				ssize_t bytes_sent = send(_sd, response_data.c_str(), response_data.size(), 0);

				if (bytes_sent < 0) {
					std::cerr << "Error sending data, errno: " << strerror(errno) << std::endl;
					_client_socket[i] = 0;
					_responses.erase(_sd);
					_connection_times.erase(_sd);
					close(_sd);
				}
				else if (bytes_sent == response_data.size()) {
					_client_socket[i] = 0;
					_responses.erase(_sd);
					_connection_times.erase(_sd);
					close(_sd);
				}
				else
					response_data = response_data.substr(bytes_sent);
			}
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