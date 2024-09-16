#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "macro.hpp"
class Server;
class Location;

class Request {
	private:
		short			_method;
		bool			_accept;
		char			_content[4096];
		std::string 	_httpRequest;
		std::string		_extension;
		std::string 	_path;
		size_t			_status_code;
		bool			_success;

		Location *_location;

		void parse_request(Server &s);
		void parse_extension(void);
		void resolvePath(Server &s);
		void throw_and_set_status(const size_t status_code, std::string message);
	public:
		Request(std::string &header, Server &s);
		~Request();
		
		const char			*getContent(void) const;
		const std::string	&getHttpRequest(void) const;
		const short			&getMethod(void) const;
		const std::string	&getExtension(void) const;
		const std::string	&getPath(void) const;
		const bool			&isAccepted(void) const;
		Location			*getLocation(void) const;
		size_t 				&get_status_code(void);
		const bool			&getSuccess(void) const;
};

std::ostream &operator<<(std::ostream &o, const Request &i);

#endif /* ********************************************************* REQUEST_H */