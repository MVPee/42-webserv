#ifndef LOCATION_HPP
# define LOCATION_HPP
# include "macro.hpp"

class Location {
	private:
		std::string _location;
		std::string	_root;
		std::string	_index;
		std::string _errorPage;
		bool		_methods[3];
	public:
		Location() : _location("/") {

		}
		~Location() {

		}
		void setLocation(std::string location) { _location = location; }
		void setRoot(std::string root) { _root = root; }
		void setIndex(std::string index) { _index = index; }
		void setErrorPage(std::string errorPage) { _errorPage = errorPage; }
		void acceptMethods(short index) { _methods[index] = true; }

		const std::string &getLocation() const { return (_location); }
		const std::string &getRoot() const { return (_root); }
		const std::string &getIndex() const { return (_index); }
		const std::string &getErrorPage() const { return (_errorPage); }
		const bool &getMethods(short index) const { return (_methods[index]); }
};

inline std::ostream &	operator<<( std::ostream & o, std::vector<Location *> &_locations) {
    int count = -1;
    while (_locations.size() > ++count) {
        o << "Location: " << _locations.at(count)->getLocation() << "\n";
        o << "Root: " << _locations.at(count)->getRoot() << "\n";
        o << "Index: " << _locations.at(count)->getIndex() << "\n";
        o << "Error_Page: " << _locations.at(count)->getErrorPage() << "\n";
        o << "POST: " << _locations.at(count)->getMethods(0) << "\n";
        o << "GET: " << _locations.at(count)->getMethods(1) << "\n";
        o << "DELETE: " << _locations.at(count)->getMethods(2) << "\n\n";
    }
	return (o);
};

#endif