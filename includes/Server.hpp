#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <vector>

# define GET 0
# define POST 1
# define DELETE 2

class Server {
	private:
		std::string name;
		unsigned int port;
		unsigned int body;
		bool methods[3];
		std::string root;
		std::string index;
		std::string error;
	
		const std::vector<std::string> Tokenize (const std::string config_text) const;

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