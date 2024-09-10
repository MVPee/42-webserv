#include "../../includes/methods/Cgi.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cgi::Cgi(Request &request) : 
_request(request),
_status_code(request.get_status_code())
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
	_path_query = (query_pos < path.size()) ? path.substr(query_pos + 1) : "";
	_folder = path_to_cgi.substr(0, path_to_cgi.find_last_of('/'));
	_executable = path_to_cgi.substr(_folder.size() + 1); 
	_path_info = (query_pos > py_pos) ? path.substr(py_pos + 3, query_pos - (py_pos + 3)) : "";

}

void Cgi::execute_cgi( void )
{
	//TODO Handle timeouts
	//TODO handle Post Request (put body in stdin)
	const std::string exec = "/usr/bin/python3";
	const std::string env_path_info = "PATH_INFO=" + _path_info;
	const std::string env_path_query = "PATH_QUERY=" + _path_query; //! or setenv ?

	char *const args[3] = {const_cast<char*>(exec.c_str()), const_cast<char*>(_executable.c_str()), NULL};
	char *const env[3] = {const_cast<char*>(env_path_info.c_str()), const_cast<char*>(env_path_query.c_str()), NULL};

	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		throw std::runtime_error("Pipe failed");

	int pid = fork();
	if (pid == 0) {
		chdir(_folder.c_str());
		close(pipe_fd[READ_PIPE]);
		dup2(pipe_fd[WRITE_PIPE], STDOUT_FILENO);
		close(pipe_fd[WRITE_PIPE]);
		execve(exec.c_str(), args, env);
		exit(EXIT_FAILURE); 
	} else { 
		receive_cgi(pipe_fd, pid);
	}
}

void Cgi::receive_cgi(int *pipe_fd, int pid)
{
	close(pipe_fd[WRITE_PIPE]);
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	while ((bytes_read = read(pipe_fd[READ_PIPE], buffer, sizeof(buffer) - 1)) > 0) {
		buffer[bytes_read] = '\0';
		std::cout << buffer;
	}
	close(pipe_fd[READ_PIPE]);
	int status;
	waitpid(pid, &status, 0);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */