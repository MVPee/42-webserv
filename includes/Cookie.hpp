#ifndef COOKIE_HPP
# define COOKIE_HPP

# include "macro.hpp"

class Server;

class Cookie {
	private:
		int _id;

		std::string find_value_by_id(const std::string &id, const std::string &request);
	public:
		Cookie(const std::string &request, std::string &response_header);
		~Cookie();

		const int &getId() const { return _id; }
};

#endif /* ********************************************************** COOKIE_H */