#include "../includes/Config.hpp"

/*
** ------------------------------- STATIC -------------------------------------
*/

static bool countAscii(std::string string) {
	for (int i = 65; i < 123; i++) {
		if (i == 91) i = 97;
		if (string.find(i) != std::string::npos)
			return 1;
	}
	return 0;
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Config::Config(std::string config) : 
	_address("0.0.0.0"),
	_port(8080),
	_body(100) {
	
	_locations.push_back(new Location);

	std::string location;
	std::string temp;

	{
		std::stringstream file(config);
		while (std::getline(file, temp)) {
			if(temp[0] == ' ' && !temp.empty() && countAscii(temp)) {
				std::string line = temp.erase(0, 4);
				if (line.find("location") != std::string::npos || line.find("    ", 0) != std::string::npos) {
					location += line + '\n';
				}
				else {
					char token[500];
					sscanf(line.c_str(), "%s", token);
					if (line[0] != '#')
						parse(std::string(token), line);
				}
			}
		}
	}

	std::stringstream file(location);
	while(std::getline(file, temp)) {
		if (temp.find("location") != std::string::npos)
			_locations.push_back(new Location);
		// std::string line = temp.erase(0, 4);
		char token[500];
		sscanf(temp.c_str(), "%s", token);
		if (temp[0] != '#')
			parseLocation(std::string(token), temp, _locations.size() - 1);

	}

	// std::cout << G << *this << C << std::endl; //* DEBUG
	// std::cout << B << _locations << C << std::endl; //* DEBUG
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Config const & i ) {
	o << "Name: " << i.getServerName() << '\n';
	o << "Address: " << i.getAddress() << '\n';
	o << "Port: " << i.getPort() << '\n';
	o << "Body: " << i.getBody() << '\n';
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Config::parse(std::string token, std::string line) {
	// std::cout << R << token << ":\t" << B << line << C << std::endl;
	char temp[3][500];

	if (token == "name") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_serverName = temp[0];
	}
	else if (token == "listen") {
		sscanf(line.c_str(), "%*s %s", temp[0]);

		std::string address_port(temp[0]);
		std::string::size_type pos = address_port.find(":");
		if (pos != std::string::npos) {
			_address = address_port.substr(0, pos);
			_port = atoi(address_port.substr(pos + 1).c_str());
		}
		std::cout << _address << std::endl;
		std::cout << _port << std::endl;
	}
	else if (token == "body_size") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_body = atol(temp[0]);
	}
	else if (token == "root") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(0)->setRoot(temp[0]);
	}
	else if (token == "index") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(0)->setIndex(temp[0]);
	}
	else if (token == "error") {
		int code;
		sscanf(line.c_str(), "%*s %i %s", &code, temp[0]);
		_locations.at(0)->setErrorPage(temp[0], code);
	}
	else if (token == "redirection") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(0)->setRedirection(temp[0]);
	}
	else if (token == "listing") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		if (std::string(temp[0]) == "ON")
			_locations.at(0)->acceptListing(true);
	}
	else if (token == "upload") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(0)->setUpload(temp[0]);
	}
	else if (token == "GET") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(0)->setCGI(temp[0], GET);
	}
	else if (token == "POST") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(0)->setCGI(temp[0], POST);
	}
	else if (token == "methods") {
		sscanf(line.c_str(), "%*s %s %s %s", temp[0], temp[1], temp[2]);
		if (std::string(temp[0]) == "GET" || std::string(temp[1]) == "GET" || std::string(temp[2]) == "GET") _locations.at(0)->acceptMethods(GET);
		if (std::string(temp[0]) == "POST" || std::string(temp[1]) == "POST" || std::string(temp[2]) == "POST") _locations.at(0)->acceptMethods(POST);
		if (std::string(temp[0]) == "DELETE" || std::string(temp[1]) == "DELETE" || std::string(temp[2]) == "DELETE") _locations.at(0)->acceptMethods(DELETE);
	}
}

void Config::parseLocation(std::string token, std::string line, size_t size) {
	char temp[3][500];

	if (token == "location") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setLocation(temp[0]);
	}
	else if (token == "root") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setRoot(temp[0]);
	}
	else if (token == "index") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setIndex(temp[0]);
	}
	else if (token == "error") {
		int code;
		sscanf(line.c_str(), "%*s %i %s", &code, temp[0]);
		_locations.at(size)->setErrorPage(temp[0], code);
	}
	else if (token == "upload") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setUpload(temp[0]);
	}
	else if (token == "GET") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setCGI(temp[0], GET);
	}
	else if (token == "POST") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setCGI(temp[0], POST);
	}
	else if (token == "redirection") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		_locations.at(size)->setRedirection(temp[0]);
	}
	else if (token == "methods") {
		sscanf(line.c_str(), "%*s %s %s %s", temp[0], temp[1], temp[2]);
		if (std::string(temp[0]) == "GET" || std::string(temp[1]) == "GET" || std::string(temp[2]) == "GET") _locations.at(size)->acceptMethods(GET);
		if (std::string(temp[0]) == "POST" || std::string(temp[1]) == "POST" || std::string(temp[2]) == "POST") _locations.at(size)->acceptMethods(POST);
		if (std::string(temp[0]) == "DELETE" || std::string(temp[1]) == "DELETE" || std::string(temp[2]) == "DELETE") _locations.at(size)->acceptMethods(DELETE);
	}
	else if (token == "listing") {
		sscanf(line.c_str(), "%*s %s", temp[0]);
		if (std::string(temp[0]) == "ON")
			_locations.at(size)->acceptListing(true);
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/



/* ************************************************************************** */