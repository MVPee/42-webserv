#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "macro.hpp"

class Server;
class Location;

class Response {
	private:
		std::string _httpRequest;
		std::string _httpResponse;
		
		size_t _status_code;
		bool _accept;

		Location *_location;

		std::string _path;
		std::string _extension;
		int _method;


		void throw_and_set_status(const size_t status_code, std::string message);
		void parse_request(const Server &s);
		void parse_extension( void );
		void resolvePath(const Server &s);
		const std::string generate_response(const std::string &page_content) const;
	public:
		Response(const Server &s, std::string full_request);
		~Response();
		const std::string &getHttpResponse(void) { return (_httpResponse); }
		const std::string &getPath(void) { return (_path); }
		size_t &get_status_code(void) { return (_status_code); }
		const bool &isAccepted(void) { return (_accept); }
		const std::string &getExtension(void) { return (_extension); }
		Location *getLocation(void) { return (_location); }
};

std::ostream &			operator<<( std::ostream & o, Response const & i );

#endif /* ******************************************************** RESPONSE_H */