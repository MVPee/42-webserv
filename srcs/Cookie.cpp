#include "../includes/Cookie.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

static std::string find_value_by_id(const std::string &id, const std::string &request) {
    std::string::size_type start = request.find("Cookie: ");
    if (start != std::string::npos) {
        std::string::size_type end = request.find("\r\n", start);
        std::string line = request.substr(start, end - start);
        start = line.find(id + "=");
        if (start != std::string::npos) {
            end = line.find(";", start);
            if (end == std::string::npos)
                end = line.size();
            std::string value = line.substr(start + id.size() + 1, end - (start + id.size() + 1));
            return (value);
        }
    }
    
    return ("");
}

Cookie::Cookie(const std::string &request) :
_id(0) {
	// std::cout << request << std::endl;
	std::string temp;
	temp = find_value_by_id("id", request);
	if (temp != "") {
		_id = atoi(temp.c_str());
	}
	else {
		std::cout << "New connection" << std::endl;
	}
	std::cout << _id << std::endl;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cookie::~Cookie() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Cookie const & i ) {
	(void)i;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */