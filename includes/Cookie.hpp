#ifndef COOKIE_HPP
# define COOKIE_HPP

# include "macro.hpp"

class Server;

class Cookie {
	private:
		int _id;
	public:
		Cookie(const std::string &request, std::string &response_header);
		~Cookie();

		std::string find_value_by_id(const std::string &id, const std::string &request);
		const int &getId() const {return (_id); }
};

std::ostream &			operator<<( std::ostream & o, Cookie const & i );

#endif /* ********************************************************** COOKIE_H */