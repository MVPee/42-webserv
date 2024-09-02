#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(const std::string config_text) :
	name("default_server"), 
	port(80), 
	body(100),
	root("./rsrcs/"),
	index("index.html"),
	error("404.html")
{
	this->methods[GET] = false;
	this->methods[POST] = false;
	this->methods[DELETE] = false;

	this->fd[SOCKET] = 0;
	this->fd[BIND] = 0;
	this->fd[LISTEN] = 0;
	this->fd[ACCEPT] = 0;

	std::map<std::string, std::string> map;

	Config config(config_text);

	name = config.getServerName();
	port = config.getPort();
	body = config.getBody();
	methods[GET] = config.getLocations().at(0)->getMethods(GET);
	methods[POST] = config.getLocations().at(0)->getMethods(POST);
	methods[DELETE] = config.getLocations().at(0)->getMethods(DELETE);
	root = config.getLocations().at(0)->getRoot();
	index = config.getLocations().at(0)->getIndex();
	error = config.getLocations().at(0)->getErrorPage();
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server() {
	if (fd[SOCKET])
		close(fd[SOCKET]);
	if (fd[BIND])
		close(fd[BIND]);
	if (fd[LISTEN])
		close(fd[LISTEN]);
	if (fd[ACCEPT])
		close(fd[ACCEPT]);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Server const & i ) {
	o << M << "\n----Server----\n";
	o << "|Name: " << i.getName() << "\n";
	o << "|Port: " << i.getPort() << "\n";
	o << "|Method GET: " << (i.getMethods(GET) ? "True" : "False") << "\n";
	o << "|Method POST: " << (i.getMethods(POST) ? "True" : "False") << "\n";
	o << "|Method DELETE: " << (i.getMethods(DELETE) ? "True" : "False") << "\n";
	o << "|Body: " << i.getBody() << "\n";
	o << "|Root: " << i.getRoot() << "\n";
	o << "|Index: " << i.getIndex() << "\n";
	o << "|Error: " << i.getError() << "\n";
	o << "-----End-----\n" << C;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Server::mySocket(void) {
	this->fd[SOCKET] = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd[SOCKET] < 0) throw std::runtime_error("Socked failed");

	this->sock_address.sin_family = AF_INET;
    this->sock_address.sin_port = htons(this->port);
    this->sock_address.sin_addr.s_addr = INADDR_ANY;
}

void Server::myBind(void) {
	std::ostringstream str1;
	int opt = 1;
    setsockopt(this->fd[SOCKET], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	this->fd[BIND] = bind(this->fd[SOCKET], (sockaddr *) &this->sock_address, sizeof(this->sock_address));
	str1 << fd[BIND];
	if (this->fd[BIND] < 0) throw std::runtime_error("Bind failed " + str1.str());
}

void Server::myListen(void) {
	this->fd[LISTEN] = listen(this->fd[SOCKET], this->body);
	if (this->fd[LISTEN] < 0) throw std::runtime_error("Listen failed");

	std::ostringstream ss;
	ss << "\n*** Listening on ADDRESS: " 
		<< inet_ntoa(sock_address.sin_addr) 
		<< " PORT: " << ntohs(sock_address.sin_port) 
		<< " ***\n\n";
	std::cout << ss.str() << std::endl; //* DEBUG
}

void Server::process(void) {
	this->fd[ACCEPT] = accept(this->fd[SOCKET], 0, 0);
	if (this->fd[ACCEPT] < 0)
		throw std::runtime_error("Accept failed");

	_request = NULL;
	_response = NULL;

	try {
		struct pollfd struct_fds = {fd[ACCEPT], POLLIN | POLLPRI | POLLOUT, 0};
		if (poll(&struct_fds, 1, 0) <= 0) throw std::runtime_error("poll failed");

		_request = new Request(fd[ACCEPT], *this);
		std::cout << B << *_request << C << std::endl; //* DEBUG
		if (_request->isAccepted())
			_response = new Response(fd[ACCEPT], *_request, *this);
	}
	catch (std::exception &e) {
		if (_request)
			delete _request;
		if (_response)
			delete _response;
		throw std::runtime_error(e.what());
	}

	if (_request)
		delete _request;
	if (_response)
		delete _response;
	close(fd[ACCEPT]);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string &Server::getName(void) const { return (this->name); }
const unsigned int &Server::getPort(void) const { return (this->port); } 
const unsigned int &Server::getBody(void) const { return (this->body); }
const std::string &Server::getRoot(void) const { return (this->root); }
const std::string &Server::getIndex(void) const { return (this->index); }
const std::string &Server::getError(void) const { return (this->error); }
const bool &Server::getMethods(unsigned int index) const {
	if (index >= 3)
        throw std::out_of_range("Index out of range");
	return (this->methods[index]);
}

/* ************************************************************************** */