#include "../../includes/methods/Cgi.hpp"

/*
** ------------------------------- STATIC --------------------------------
*/


const char* get_exec_command(const std::string &extension) {
	if (extension == ".py") return "/usr/bin/python3";
	else if (extension == ".php") return "php";
	else return "";
}


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
	o << "CgiExtension: " << i.getCgiExtension() << "\n";
	o << "Folder: " << i.getFolder() << "\n";
	o << "Path_Info: " << i.getPath_info() << "\n";
	o << "Path_Query: " << i.getPath_query() << "\n";
	o << "------End------\n" << C;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/
void Cgi::get_cgi_infos( void )
{
	std::string path = _request.getPath();

	std::size_t query_pos = path.find('?');
	
	std::size_t extension_pos = find_cgi_extension(path);

	std::size_t end_of_extension = path.find_first_of("./?", extension_pos + 1);
	_cgi_extension = path.substr(extension_pos, ((end_of_extension != std::string::npos) ? end_of_extension : path.size()) - extension_pos);
	std::size_t extension_size = _cgi_extension.size();

	if (extension_pos == std::string::npos) extension_pos = path.size();
	if (query_pos == std::string::npos) query_pos = path.size();

	std::string path_to_cgi = path.substr(0, extension_pos + extension_size);
	_path_query = (query_pos < path.size()) ? path.substr(query_pos) : "";
	_folder = path_to_cgi.substr(0, path_to_cgi.find_last_of('/'));
	_executable = path_to_cgi.substr(_folder.size() + 1); 
	_path_info = (query_pos > extension_pos) ? path.substr(extension_pos + extension_size, query_pos - (extension_pos + extension_size)) : "";

}

void Cgi::execute_cgi( void )
{
	const std::string exec = get_exec_command(_cgi_extension);

	char *const args[3] = {const_cast<char*>(exec.c_str()), const_cast<char*>(_executable.c_str()), NULL};
	// char *const args[] = {const_cast<char*>("ubuntu_cgi_tester"), NULL};

	int std_out = dup(STDOUT_FILENO);
	int std_in = dup(STDIN_FILENO);
	int pipe_fd[2], pipe_fd2[2];
	if (pipe(pipe_fd) == -1 || pipe(pipe_fd2) == -1)
		throw std::runtime_error("Pipe failed");

    int flags = fcntl(pipe_fd[READ_PIPE], F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("FCNTL");
    }

    if (fcntl(pipe_fd[READ_PIPE], F_SETFL, flags | O_NONBLOCK) == -1) 
        throw std::runtime_error("FCNTL");

	int pid = fork();
	if (pid == -1)
	     throw std::runtime_error("fork");	
	else if (pid == 0) {
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
		// execve("ubuntu_cgi_tester", args,const_cast<char* const*>(_env.data()));
		// std::cerr << "FAIL EXECVE" << std::endl; //*DEBUG
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
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	time_t start = time(NULL);
	while (difftime(time(NULL), start) < TIME_OUT_CGI) {
		bytes_read = read(pipe_fd[READ_PIPE], buffer, sizeof(buffer) - 1);

		if (bytes_read > 0)
		{
			std::cerr << bytes_read << std::endl;
			_response_content.append(buffer, bytes_read);
			if (bytes_read < (sizeof(buffer) - 1))
				break;
		}
	}
	if (difftime(time(NULL), start) >= TIME_OUT_CGI)
	{
		kill(pid, SIGINT);
		_status_code = ERROR_REQUEST_TIMEOUT;
	}
	close(pipe_fd[READ_PIPE]);
	int status;
	waitpid(pid, &status, WNOHANG);
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) != 0)
			_status_code = ERROR_INTERNAL_SERVER;
	}
	// std::cerr << Y << _response_content << C << std::endl; //*DEBUG
}

void Cgi::generate_env ( void )
{

	add_env_variable("PATH_INFO", _path_info);
	add_env_variable("PATH_QUERY", _path_query);
	add_env_variable("CONTENT_LENGTH", ft_to_string(_body.size()));
	add_env_variable("SERVER_PROTOCOL", HTML_VERSION);
	add_env_variable("REQUEST_METHOD", (_request.getMethod() == GET) ? "GET" : "POST");
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