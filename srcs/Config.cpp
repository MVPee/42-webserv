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

static bool isValidIPAddress(const std::string& ip) {
    std::istringstream iss(ip);
    std::string segment;
    int num;
    int count = 0;

    while (std::getline(iss, segment, '.')) {
		num = atoi(segment.c_str());
        if (num < 0 || num > 255) return false;
        count++;
    }
    return count == 4;
}

void Config::parse(std::string token, std::string line) {
    std::istringstream iss(line);
    std::string value;

    iss >> value;

    if (token == "name")
        iss >> _serverName;
    else if (token == "listen") {
        iss >> value;
        std::string::size_type pos = value.find(":");
        if (pos != std::string::npos) {
            _address = value.substr(0, pos);
			if (!isValidIPAddress(_address)) {
				std::cerr << Y << "Address is invalid (set to 0.0.0.0)" << C << std::endl;
				_address = "0.0.0.0";
			}
            _port = atoi(value.substr(pos + 1).c_str());
			if (_port > 65535 || value.substr(pos + 1).length() > 5) {
				std::cerr << Y << "Port is too hight or invalid (set to 8080)" << C << std::endl;
				_port = 8080;
			}
        }
    }
    else if (token == "body_size") {
        iss >> value;
        _body = atol(value.c_str());
    }
    else if (token == "root") {
        iss >> value;
        _locations.at(0)->setRoot(value);
    }
    else if (token == "index") {
        iss >> value;
        _locations.at(0)->setIndex(value);
    }
    else if (token == "error") {
        int code;
        iss >> code >> value;
        _locations.at(0)->setErrorPage(value, code);
    }
    else if (token == "redirection") {
        iss >> value;
        _locations.at(0)->setRedirection(value);
    }
    else if (token == "listing") {
        iss >> value;
        if (value == "ON")
            _locations.at(0)->acceptListing(true);
    }
    else if (token == "upload") {
        iss >> value;
        _locations.at(0)->setUpload(value);
    }
    else if (token == "GET") {
        iss >> value;
        _locations.at(0)->setCGI(value, GET);
    }
    else if (token == "POST") {
        iss >> value;
        _locations.at(0)->setCGI(value, POST);
    }
    else if (token == "methods") {
        std::string method1, method2, method3;
        iss >> method1 >> method2 >> method3;

        if (method1 == "GET" || method2 == "GET" || method3 == "GET") 
            _locations.at(0)->acceptMethods(GET);
        if (method1 == "POST" || method2 == "POST" || method3 == "POST") 
            _locations.at(0)->acceptMethods(POST);
        if (method1 == "DELETE" || method2 == "DELETE" || method3 == "DELETE") 
            _locations.at(0)->acceptMethods(DELETE);
    }
}

void Config::parseLocation(std::string token, std::string line, size_t size) {
    std::istringstream iss(line);
    std::string value;

    iss >> value;

    if (token == "location") {
        iss >> value;
		if (value[0] != '/')
			value = '/' + value;
        _locations.at(size)->setLocation(value);
    }
    else if (token == "root") {
        iss >> value;
        _locations.at(size)->setRoot(value);
    }
    else if (token == "index") {
        iss >> value;
        _locations.at(size)->setIndex(value);
    }
    else if (token == "error") {
        int code;
        iss >> code >> value;
        _locations.at(size)->setErrorPage(value, code);
			
    }
    else if (token == "upload") {
        iss >> value;
        _locations.at(size)->setUpload(value);
    }
    else if (token == "GET") {
        iss >> value;
        _locations.at(size)->setCGI(value, GET);
    }
    else if (token == "POST") {
        iss >> value;
        _locations.at(size)->setCGI(value, POST);
    }
    else if (token == "redirection") {
        iss >> value;
        _locations.at(size)->setRedirection(value);
    }
    else if (token == "methods") {
        std::string method1, method2, method3;
        iss >> method1 >> method2 >> method3;

        if (method1 == "GET" || method2 == "GET" || method3 == "GET")
            _locations.at(size)->acceptMethods(GET);
        if (method1 == "POST" || method2 == "POST" || method3 == "POST")
            _locations.at(size)->acceptMethods(POST);
        if (method1 == "DELETE" || method2 == "DELETE" || method3 == "DELETE")
            _locations.at(size)->acceptMethods(DELETE);
    }
    else if (token == "listing") {
        iss >> value;
        if (value == "ON")
            _locations.at(size)->acceptListing(true);
    }
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/



/* ************************************************************************** */