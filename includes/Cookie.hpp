#ifndef COOKIE_HPP
# define COOKIE_HPP

# include "macro.hpp"

class Server;

class Cookie {
	private:
		int _id;
	public:
		Cookie(const std::string &request, Server &s);
		~Cookie();

		const int &getId() const {return (_id); }
};

std::ostream &			operator<<( std::ostream & o, Cookie const & i );

#endif /* ********************************************************** COOKIE_H */