#ifndef GET_HPP
# define GET_HPP

# include "../macro.hpp"


template <typename T>
std::string ft_to_string(T value)
{
	std::ostringstream transformer;
	transformer << value;
	return (transformer.str());
}

class Get {
	private:
		const int		_client_fd;
		const Request	&_request;
		const Server	&_server;
		int				_status_code;
		int				_fd;
		std::string		_content;

		void getContent(Request &request, Server &server);
		void generate_response(size_t status_code, Request &request);
		void generate_listing(size_t status_code, Request &request);
	public:
		Get(const int client_fd, Request &request, Server &server, size_t status_code);

		~Get();
};

std::ostream &			operator<<( std::ostream & o, Get const & i );

#endif /* ************************************************************* GET_H */