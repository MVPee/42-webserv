#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <map>
# include <cstdlib>

# define GET 0
# define POST 1
# define DELETE 2
# define R "\x1b[1;31m"
# define G "\x1b[1;32m"
# define B "\x1b[1;36m"
# define Y "\x1b[1;33m"
# define M "\x1b[1;35m"
# define C "\x1b[0m"


class Server {
	private:
		std::string name;
		unsigned int port;
		unsigned int body;
		bool methods[3];
		std::string root;
		std::string index;
		std::string error;
	
		const std::map<std::string, std::string> Tokenize (const std::string config_text) const;

	public:
		Server(const std::string config_text);
		~Server();

		const std::string &getName(void) const;
		const unsigned int &getPort(void) const;
		const unsigned int &getBody(void) const;
		const bool &getMethods(unsigned int index) const;
		const std::string &getRoot(void) const;
		const std::string &getIndex(void) const;
		const std::string &getError(void) const;
};

std::ostream &operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */