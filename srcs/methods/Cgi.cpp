#include "../../includes/methods/Cgi.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cgi::Cgi(Client &client) : 
_request(client.getRequest()),
_status_code(client.getRequest().get_status_code()),
_body(client.getBody())
{
	try
	{
		get_cgi_infos();
		execute_cgi();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

}



/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cgi::~Cgi()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Cgi const & i )
{
	o << G << "----CGI-----\n";
	o << "Executable: " << i.getExecutable() << "\n";
	o << "Folder: " << i.getFolder() << "\n";
	o << "Path_Info " << i.getPath_info() << "\n";
	o << "Path_Query " << i.getPath_query() << "\n";
	o << "------End------\n" << C;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/
void Cgi::get_cgi_infos( void )
{
	std::string path = _request.getPath();

	std::size_t py_pos = path.find(".py");
	std::size_t query_pos = path.find('?');

	if (py_pos == std::string::npos) py_pos = path.size();
	if (query_pos == std::string::npos) query_pos = path.size();

	// Extract path components
	std::string path_to_cgi = path.substr(0, py_pos + 3);
	_path_query = (query_pos < path.size()) ? path.substr(query_pos) : "";
	_folder = path_to_cgi.substr(0, path_to_cgi.find_last_of('/'));
	_executable = path_to_cgi.substr(_folder.size() + 1); 
	_path_info = (query_pos > py_pos) ? path.substr(py_pos + 3, query_pos - (py_pos + 3)) : "";

}

void Cgi::execute_cgi( void )
{
	//TODO Handle timeouts
	const std::string exec = "/usr/bin/python3";
	const std::string env_path_info = "PATH_INFO=" + _path_info;
	const std::string env_path_query = "PATH_QUERY=" + _path_query; //! or setenv ?
	const std::string env_content_length = "CONTENT_LENGTH=" + ft_to_string(_body.size());

	char *const args[3] = {const_cast<char*>(exec.c_str()), const_cast<char*>(_executable.c_str()), NULL};
	char *const env[4] = {const_cast<char*>(env_path_info.c_str()), const_cast<char*>(env_path_query.c_str()), const_cast<char*>(env_content_length.c_str()), NULL};

	int std_out = dup(STDOUT_FILENO);
	int std_in = dup(STDIN_FILENO);
	int pipe_fd[2], pipe_fd2[2];
	if (pipe(pipe_fd) == -1 || pipe(pipe_fd2) == -1)
		throw std::runtime_error("Pipe failed");

	int pid = fork();
	if (pid == 0) {
		chdir(_folder.c_str());
		dup2(pipe_fd[WRITE_PIPE], STDOUT_FILENO);
		close(pipe_fd[WRITE_PIPE]);
		if (_request.getMethod() == POST)
		{
			dup2(pipe_fd2[READ_PIPE], STDIN_FILENO);
		}
		close(pipe_fd2[READ_PIPE]);
		close(pipe_fd2[WRITE_PIPE]);
		close(pipe_fd[READ_PIPE]);
		execve(exec.c_str(), args, env);
		exit(EXIT_FAILURE); 
	} else { 
		receive_cgi(pipe_fd, pipe_fd2, pid);
	}
	dup2(std_out, STDOUT_FILENO);
	close(std_out);
	dup2(std_in, STDIN_FILENO);
	close(std_in);
}

void Cgi::receive_cgi(int *pipe_fd, int *pipe_fd2, int pid)
{
	close(pipe_fd2[READ_PIPE]);
	if (_request.getMethod() == POST)
		write(pipe_fd2[WRITE_PIPE], _body.c_str(), _body.size());
	close(pipe_fd2[WRITE_PIPE]);
	close(pipe_fd[WRITE_PIPE]);
	// sleep(2);
	// kill(pid, SIGINT);
	char buffer[1024];
	ssize_t bytes_read;
	while ((bytes_read = read(pipe_fd[READ_PIPE], buffer, sizeof(buffer) - 1)) > 0) {
		_response_content.append(buffer, bytes_read);
	}
	close(pipe_fd[READ_PIPE]);
	int status;
	waitpid(pid, &status, 0);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */