#ifndef CGI_HPP
# define CGI_HPP

# include "../macro.hpp"
//? temporary
size_t find_cgi_extension (const std::string &path);

class Client;

class Cgi
{
	private:
		size_t						&_status_code;
		Request 					&_request;
		const std::string			&_body;
		std::vector<const char*>	_env;

		std::string		_executable;
		std::string		_path_query;
		std::string		_path_info;
		std::string		_folder;
		std::string		_cgi_extension;
		std::string		_response_content;

		int				_save_std_out;
		int				_save_std_in;
		int				_pipe_fd[2];
		int				_pipe_fd2[2];

		void get_cgi_infos( void );
		void execute_cgi( void );
		void receive_cgi(int *pipe_fd, int *pipe_fd2, int pid);
		void generate_env ( void );
		void add_env_variable (const std::string &name, const std::string &value);
		void clean ( void );

	public:

		Cgi(Client &client);
		~Cgi();

		const std::string & getExecutable( void ) const {return(_executable);}
		const std::string & getPath_query( void ) const {return(_path_query);}
		const std::string & getPath_info( void ) const {return(_path_info);}
		const std::string & getFolder( void ) const {return(_folder);}
		const std::string & getCgiExtension( void ) const {return(_cgi_extension);}
		const std::string & getResponseContent( void ) const {return(_response_content);}


};

std::ostream &			operator<<( std::ostream & o, Cgi const & i );
#endif /* ************************************************************* CGI_H */