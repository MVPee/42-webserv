#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(const std::string config_text) :
	name("default"), 
	port(80), 
	body(100) {
	methods[GET] = false;
	methods[POST] = false;
	methods[DELETE] = false;


	try {
		Tokenize(config_text);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	// if (tokens.empty() || tokens.size() < 3)
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server() {
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Server const & i ) {
	(void) i;
	// o << "Name: " << i.getName();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/



const std::vector<std::string> &Server::Tokenize (const std::string config_text) const
{
	std::string::const_iterator it = config_text.begin();
	std::string::const_iterator ite = config_text.end();
	std::vector<std::string> tokens;
	std::string current_token;
	size_t current_level = 0;

	while (it != ite)
	{
		if (*it == '{') current_level++;
		else if (*it == '}') current_level--;

		if (std::isspace(*it) && !current_token.empty()) {
			tokens.push_back(current_token);
			current_token.clear();
		}
		else if (!std::isspace(*it))
			current_token.push_back(*it);
		it++;
	}
	if (current_level != 0) throw std::runtime_error("wrong level"); //TODO: meilleur message
	if (!current_token.empty())
		tokens.push_back(current_token);

	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << std::endl;

	return (tokens);
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