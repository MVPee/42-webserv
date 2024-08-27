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
	error("404.html") {
	methods[GET] = false;
	methods[POST] = false;
	methods[DELETE] = false;

	std::map<std::string, std::string> map;

	try {
		map = Tokenize(config_text);
		if (map.count("name")) this->name = map["name"]; 
		if (map.count("port")) this->port = std::atoll(map["port"].c_str());
		if (map.count("body_size")) this->body = std::atoll(map["body_size"].c_str());
		if (map.count("root")) this->root = map["root"];
		if (map.count("index")) this->index = map["index"];
		if (map.count("error_page")) this->error = map["error_page"];
		if (map.count("methods")) {
			std::stringstream s;
			std::string line;
			s << map["methods"];
			while (std::getline(s, line, ' ')) {
				line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
				std::cout << Y << "'" + line + "'" << C << std::endl; //* DEBUG
				if (line == "GET") this->methods[GET] = true;
				else if (line == "POST") this->methods[POST] = true;
				else if (line == "DELETE") this->methods[DELETE] = true;
			}
		}
	}
	catch (std::exception &e) {
		std::cerr << R << e.what() << C << std::endl;
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
	o << M << "\n---------\n";
	o << "|Name: " << i.getName() << "\n";
	o << "|Port: " << i.getPort() << "\n";
	o << "|Method GET: " << (i.getMethods(GET) ? "True" : "False") << "\n";
	o << "|Method POST: " << (i.getMethods(POST) ? "True" : "False") << "\n";
	o << "|Method DELETE: " << (i.getMethods(DELETE) ? "True" : "False") << "\n";
	o << "|Body: " << i.getBody() << "\n";
	o << "|Root: " << i.getRoot() << "\n";
	o << "|Index: " << i.getIndex() << "\n";
	o << "|Error: " << i.getError() << "\n";
	o << "---------\n" << C;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/



const std::map<std::string, std::string> Server::Tokenize (const std::string config_text) const
{
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
			first_line = false;
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
			if (line.compare(0, 6, "server") != 0) throw std::runtime_error("missing 'server' label");
			else if (line.length() > 6 ) line.erase(0, 6);
			else continue;
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

	// std::map<std::string, std::string>::const_iterator it = map.begin(); //* Debug
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