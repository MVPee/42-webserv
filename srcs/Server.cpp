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
	//todo: put more defaults
	std::map<std::string, std::string> map;

	try {
		map = Tokenize(config_text);
		if (map.count("name")) this->name = map["name"]; 
		if (map.count("port"))this->port = std::atoll(map["port"].c_str());
		if (map.count("body_size"))this->body = std::atoll(map["body_size"].c_str());
		if (map.count("root"))this->root = map["root"];
		if (map.count("index"))this->index = map["index"];
		if (map.count("error_page"))this->error = map["error_page"];
		// if (map.count("methods")) //Todo: add methods
		// {
			
		// }
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
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
	o << "Name: " << i.getName() << "\n";
	o << "Port: " << i.getPort() << "\n";
	o << "Method GET: " << (i.getMethods(GET) ? "True" : "False") << "\n";
	o << "Method POST: " << (i.getMethods(POST) ? "True" : "False") << "\n";
	o << "Method DELETE: " << (i.getMethods(DELETE) ? "True" : "False") << "\n";
	o << "Body: " << i.getBody() << "\n";
	o << "Root: " << i.getRoot() << "\n";
	o << "Index: " << i.getIndex() << "\n";
	o << "Error: " << i.getError() << "\n";
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/



const std::map<std::string, std::string> Server::Tokenize (const std::string config_text) const
{
	//? Passer directement le infile
	std::stringstream stream(config_text);
	std::map<std::string, std::string> map;
	std::string line;
	bool first_line = true;
	size_t i = 0;
	size_t start = 0;
	long current_level = 0;

	while (std::getline(stream, line))
	{
		i = 0;
		if (first_line)
		{
			if (line.compare(0, 6, "server") != 0) throw std::runtime_error("missing 'server' label");
			line.erase(0, 6); 
			first_line = false;
		}
		while (i < line.length() && isspace(line[i])) i++;
		start = i;
		if (i < line.length() && line[i] == '{') current_level++;
		else if (i < line.length() && line[i] == '}' && current_level > 0) current_level--;
		else if  (i < line.length() && line[i] == '}' && current_level <= 0) throw std::runtime_error("wrong level");
		else
		{
			while (i < line.length() && !isspace(line[i])) i++;
			if (i + 1 >= line.length()) throw std::runtime_error("empty data");
			map.insert(std::pair<std::string, std::string>(line.substr(start, i - start), line.substr(i + 1, line.length() - i)));
		}
	}
	if (current_level != 0) throw std::runtime_error("wrong level"); //TODO: meilleur message

	// std::map<std::string, std::string>::const_iterator it = map.begin(); //? Debug
	// while(it != map.end())
	// {
	// 	std::cout << it->first << "|" << it->second << "\n";
	// 	it++;
	// }

	return (map);
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