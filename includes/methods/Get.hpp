#ifndef GET_HPP
# define GET_HPP

# include "../macro.hpp"


template <typename T>
std::string ft_to_string(T value) {
	std::ostringstream transformer;
	transformer << value;
	return transformer.str();
}
/**
 * @param status_code the error code (for example: 404)
 * 
 * @return the string meaning of the error code (for example: 'PAGE NOT FOUND')
 */
const char* get_status_message(const size_t status_code);

class Get {
	private:
		const Request	&_request;
		const Server	&_server;
		size_t			&_status_code;
		std::string		_content;

		void getContent(void);
		void get_file(void);
		void generate_listing(void);
		void generate_redirection(std::string redirection);
	public:
		Get(Request &request, Server &server);
		const std::string &get_content(void) const;

		~Get();
};

std::ostream &			operator<<( std::ostream & o, Get const & i );

#endif /* ************************************************************* GET_H */