#ifndef SERVER_HPP
# define SERVER_HPP

# include "macro.hpp"

class Server {
	private:
		std::string name;
		unsigned int port;
		unsigned int body;
		bool methods[3];
		std::string root;
		std::string index;
		std::string error;

		sockaddr_in sock_address;
		int fd[6];
		char receive_buffer[1024];
	
		const std::map<std::string, std::string> tokenize (const std::string config_text) const;
		void clearFileDescriptor();
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

		void mySocket(void);
		void myBind(void);
		void myListen(void);
		void myAccept(void);
		void myReceive(void);
		void mySend(void);

		std::string getFile(std::string page);
};

std::ostream &operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */