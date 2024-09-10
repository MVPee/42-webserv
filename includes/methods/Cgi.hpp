#ifndef CGI_HPP
# define CGI_HPP

# include "../macro.hpp"

class Cgi
{
	private:
		size_t			&_status_code;
		Request 		&_request;
		std::string		_executable;
		std::string		_path_query;
		std::string		_path_info;
		std::string		_folder;

		void get_cgi_infos( void );
		void execute_cgi( void );
		void receive_cgi(int *pipe_fd, int pid);

	public:

		Cgi(Request &request);
		~Cgi();

		const std::string & getExecutable( void ) const {return(_executable);};
		const std::string & getPath_query( void ) const {return(_path_query);};
		const std::string & getPath_info( void ) const {return(_path_info);};
		const std::string & getFolder( void ) const {return(_folder);};


};

std::ostream &			operator<<( std::ostream & o, Cgi const & i );
#endif /* ************************************************************* CGI_H */