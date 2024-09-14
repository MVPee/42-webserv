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
		generate_env();
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
	while (!_env.empty())
	{
		free((void *)(_env.back()));
		_env.pop_back();
	}
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

	char *const args[3] = {const_cast<char*>(exec.c_str()), const_cast<char*>(_executable.c_str()), NULL};

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
		execve(exec.c_str(), args,const_cast<char* const*>(_env.data()));
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

void Cgi::generate_env ( void )
{

	add_env_variable("PATH_INFO", _path_info);
	add_env_variable("PATH_QUERY", _path_query);
	add_env_variable("CONTENT_LENGTH", ft_to_string(_body.size()));
	_env.push_back(NULL);
}

void Cgi::add_env_variable (const std::string &name, const std::string &value)
{
	_env.push_back(strdup((name + "=" + value).c_str()));
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */