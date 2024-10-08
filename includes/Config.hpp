#ifndef LOCATION_HPP
# define LOCATION_HPP
# include "macro.hpp"

class Location {
	private:
		std::string	_location;
		std::string	_root;
		std::string	_index;
		std::string _errorPage[305];
		std::string _upload;
		std::string	_redirection;
		std::string _cgi[MAX_CGI_EXT];
		bool		_methods[3];
		bool		_listing;
	public:
		Location() : _location("/"), _root("NONE"), _index(""), _upload(""), _listing(false) {
			_methods[GET] = false;
			_methods[POST] = false;
			_methods[DELETE] = false;
			for (size_t i = 0; i < MAX_CGI_EXT; i++)
				_cgi[i].clear();
		}
		~Location() {}
		void setLocation(std::string location) { _location = location; }
		void setRoot(std::string root) { _root = root; }
		void setIndex(std::string index) { _index = index; }
		void setErrorPage(std::string errorPage, int code) { if (code - 200 <= 305) _errorPage[code - 200] = errorPage; }
		void setUpload(std::string upload) { _upload = upload; }
		void setCGI(short index, std::string value) { _cgi[index] = value; }
		void setRedirection(std::string redirection) { _redirection = redirection; }
		void acceptMethods(short index) { _methods[index] = true; }
		void acceptListing(bool value) { _listing = value;}

		const std::string &getLocation(void) const { return _location; }
		const std::string &getRoot(void) const { return _root; }
		const std::string &getIndex(void) const { return _index; }
		const std::string &getErrorPage(int code) const { return _errorPage[code - 200]; }
		const std::string *getCGI() const { return _cgi; }
		const std::string &getUpload(void) const { return _upload; }
		const std::string &getRedirection(void) const { return _redirection; }
		const bool &getMethods(short index) const { return _methods[index]; }
		const bool &getListing(void) const { return _listing; }
};

inline std::ostream &operator<<( std::ostream & o, std::vector<Location *> &_locations) {
    size_t count = -1;
    while (_locations.size() > ++count) {
        o << "Location: " << _locations.at(count)->getLocation() << "\n";
        o << "Root: " << _locations.at(count)->getRoot() << "\n";
        o << "Index: " << _locations.at(count)->getIndex() << "\n";
        o << "POST: " << _locations.at(count)->getMethods(0) << "\n";
        o << "GET: " << _locations.at(count)->getMethods(1) << "\n";
        o << "DELETE: " << _locations.at(count)->getMethods(2) << "\n\n";
    }
	return o;
};

#endif

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "macro.hpp"

class Location;

class Config {
	private:
		std::string 				_serverName;
		std::string					_address;
		int							_port;
		long						_body;
		std::vector<Location *> 	_locations;

		void parse(std::string token, std::string line);
		void parseLocation(std::string token, std::string line, size_t size);
	public:
		Config(std::string config);
		~Config();

		const std::string &getServerName(void) const { return _serverName; }		
		const std::string &getAddress(void) const { return _address; }
		const int &getPort(void) const { return _port; }
		const long &getBody(void) const { return _body; }
		const std::vector<Location *> &getLocations(void) const { return _locations; }
};

std::ostream &operator<<(std::ostream &o, const Config &i);

#endif /* ********************************************************** CONFIG_H */

