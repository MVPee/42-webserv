#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "macro.hpp"
# include "Location.hpp"

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
};

std::ostream &			operator<<( std::ostream & o, Config const & i );

#endif /* ********************************************************** CONFIG_H */