#ifndef LOCATION_HPP
# define LOCATION_HPP
# include "macro.hpp"

class Location {
	private:
		std::string _location;
		std::string	_root;
		std::string	_index;
		std::string _errorPage[305];
		std::string _upload;
		std::string	_redirection;
		std::string _cgi[2];
		bool		_methods[3];
		bool		_listing;
	public:
		Location() : _location("/"), _index(""), _root("NONE"), _upload(""), _listing(false){
			_methods[GET] = false;
			_methods[POST] = false;
			_methods[DELETE] = false;
			_cgi[GET].clear();
			_cgi[POST].clear();
		}
		~Location() {

		}
		void setLocation(std::string location) { _location = location; }
		void setRoot(std::string root) { _root = root; }
		void setIndex(std::string index) { _index = index; }
		void setErrorPage(std::string errorPage, int code) { if (code - 200 <= 305) _errorPage[code - 200] = errorPage; }
		void setUpload(std::string upload) { _upload = upload; }
		void setCGI(std::string cgi, short index) { if (index == GET || index == POST )_cgi[index] = cgi; }
		void setRedirection(std::string redirection) { _redirection = redirection; }
		void acceptMethods(short index) { _methods[index] = true; }
		void acceptListing(bool value) { _listing = value;}

		const std::string &getLocation() const { return (_location); }
		const std::string &getRoot() const { return (_root); }
		const std::string &getIndex() const { return (_index); }
		const std::string &getErrorPage(int code) const { return (_errorPage[code - 200]); }
		const std::string &getCGI(int code) const { return (_cgi[code]); }
		const std::string &getUpload() const { return (_upload); }
		const std::string &getRedirection() const { return (_redirection); }
		const bool &getMethods(short index) const { return (_methods[index]); }
		const bool &getListing() const { return (_listing); }
};

inline std::ostream &	operator<<( std::ostream & o, std::vector<Location *> &_locations) {
    int count = -1;
    while (_locations.size() > ++count) {
        o << "Location: " << _locations.at(count)->getLocation() << "\n";
        o << "Root: " << _locations.at(count)->getRoot() << "\n";
        o << "Index: " << _locations.at(count)->getIndex() << "\n";
        o << "POST: " << _locations.at(count)->getMethods(0) << "\n";
        o << "GET: " << _locations.at(count)->getMethods(1) << "\n";
        o << "DELETE: " << _locations.at(count)->getMethods(2) << "\n\n";
    }
	return (o);
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
		int							_body;
		std::vector<Location *> 	_locations;

		void parse(std::string token, std::string line);
		void parseLocation(std::string token, std::string line, size_t size);
	public:
		Config(std::string config);
		~Config();

		const std::string &getServerName() const { return (_serverName); }		
		const std::string &getAddress() const { return (_address); }
		const int &getPort() const { return (_port); }
		const int &getBody() const { return (_body); }
		const std::vector<Location *> &getLocations() const { return (_locations); }
};

std::ostream &			operator<<( std::ostream & o, Config const & i );

#endif /* ********************************************************** CONFIG_H */

