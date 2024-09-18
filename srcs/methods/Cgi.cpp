#include "../../includes/methods/Cgi.hpp"

/*
** ------------------------------- STATIC --------------------------------
*/

size_t find_cgi_extension(const std::string &path) {
	std::string extensions[] = {".py", ".php", ".pl"};
	for (size_t i = 0; i < sizeof(extensions) / sizeof(std::string); i++) {
		size_t pos = path.find(extensions[i]);
		if (pos != std::string::npos) return pos;
	}
	return std::string::npos;
}

static const char* get_exec_command(const std::string &extension) {
	if (extension == ".py") return "/usr/bin/python3";
	else if (extension == ".php") return "/usr/bin/php";
	else if (extension == ".pl") return "/usr/bin/perl";
	else return "./";
}

static int close_and_change_value(int &fd) {
	int ret = 0;
	if (fd >= 0) {
		ret = close(fd);
		fd = -1;
	}
	return ret;
}

static void exit_error_and_print(std::string Error) {
	perror(Error.c_str());
	exit(EXIT_FAILURE);
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cgi::Cgi(Client &client) : 
_request(client.getRequest()),
_status_code(client.getRequest().get_status_code()),
_body(client.getBody()),
_save_std_in(-1),
_save_std_out(-1) {
	try {
		_pipe_fd[0] = -1;
		_pipe_fd[1] = -1;
		_pipe_fd2[0] = -1;
		_pipe_fd2[1] = -1;

		get_cgi_infos();
		generate_env();

		_save_std_out = dup(STDOUT_FILENO);
		_save_std_in = dup(STDIN_FILENO);

		if (_save_std_out == -1 || _save_std_in == -1)
			throw std::runtime_error("Dup of STDOUT or STDIN failed");
		if (pipe(_pipe_fd) == -1 || pipe(_pipe_fd2) == -1)
			throw std::runtime_error("Pipe failed");
	
		int flags = fcntl(_pipe_fd[READ_PIPE], F_GETFL, 0);
		if (flags == -1)
			throw std::runtime_error("Fcntl failed");

		if (fcntl(_pipe_fd[READ_PIPE], F_SETFL, flags | O_NONBLOCK) == -1) 
			throw std::runtime_error("Fcntl failed");

		execute_cgi();
	}
	catch(const std::exception& e) {
		std::cerr << R << e.what() << C << '\n';
		if (_status_code < 400) _status_code = ERROR_INTERNAL_SERVER;
		if (_response_content.empty())
			_response_content = "<h1>default: " + ft_to_string(_status_code) + " " + get_status_message(_status_code) + "</h1>";
	}
}



/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cgi::~Cgi() {
	while (!_env.empty()) {
		free((void *)(_env.back()));
		_env.pop_back();
	}
	clean();
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &operator<<( std::ostream & o, Cgi const & i ) {
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
void Cgi::get_cgi_infos(void) {
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

void Cgi::execute_cgi(void) {
	const std::string exec = get_exec_command(_cgi_extension);

	char *const args[3] = {const_cast<char*>(exec.c_str()), const_cast<char*>(_executable.c_str()), NULL};
	std::cout << R << (exec + " " + _folder + "/" + _executable ) << C << std::endl; //* DEBUG
	if (access((_folder + "/" + _executable).c_str(), F_OK) != 0) {
		_status_code = ERROR_NOT_FOUND;
		throw std::runtime_error("File does not exist");
	}
	//? temporary
	if (_request.getMethod() == POST && _status_code == NO_CONTENT)
		_status_code = OK;

	int pid = fork();
	if (pid == -1)
	     throw std::runtime_error("fork failed");	
	else if (pid == 0) {
		if (chdir(_folder.c_str()) == -1)
			exit_error_and_print("Chdir failed");
		if (dup2(_pipe_fd[WRITE_PIPE], STDOUT_FILENO) == -1)
			exit_error_and_print("Dup2 failed");
		if (close_and_change_value(_pipe_fd[WRITE_PIPE]) != 0)
			exit_error_and_print("Close failed");
		if (_request.getMethod() == POST)
			if (dup2(_pipe_fd2[READ_PIPE], STDIN_FILENO))
				exit_error_and_print("Dup2 failed");
		if (close_and_change_value(_pipe_fd2[READ_PIPE]) != 0)
			exit_error_and_print("Close failed");
		if (close_and_change_value(_pipe_fd2[WRITE_PIPE]) != 0)
			exit_error_and_print("Close failed");
		if (close_and_change_value(_pipe_fd[READ_PIPE]) != 0)
			exit_error_and_print("Close failed");
		execve(exec.c_str(), args,const_cast<char* const*>(_env.data()));
		exit_error_and_print("Execve failed");
	}
	else receive_cgi(_pipe_fd, _pipe_fd2, pid);
}

void Cgi::receive_cgi(int *pipe_fd, int *pipe_fd2, int pid) {
	close_and_change_value(pipe_fd2[READ_PIPE]);
	if (_request.getMethod() == POST)
		write(pipe_fd2[WRITE_PIPE], _body.c_str(), _body.size());
	close_and_change_value(pipe_fd2[WRITE_PIPE]);
	close_and_change_value(pipe_fd[WRITE_PIPE]);
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	time_t start = time(NULL);
	while (difftime(time(NULL), start) < TIME_OUT_CGI) {
		bytes_read = read(pipe_fd[READ_PIPE], buffer, sizeof(buffer) - 1);

		if (bytes_read > 0) {
			//std::cerr << bytes_read << std::endl; //*DEBUG
			_response_content.append(buffer, bytes_read);
			if (bytes_read < (sizeof(buffer) - 1))
				break;
		}
		else if (bytes_read == 0) {
			std::cerr << "Child process closed connection " << std::endl; //*DEBUG
			break;
		}
	}
	if (difftime(time(NULL), start) >= TIME_OUT_CGI) {
		kill(pid, SIGINT);
		_status_code = ERROR_REQUEST_TIMEOUT;
		_response_content = "<h1>default: " + ft_to_string(_status_code) + " " + get_status_message(_status_code) + "</h1>";
	}
	close_and_change_value(pipe_fd[READ_PIPE]);
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			_status_code = ERROR_INTERNAL_SERVER;
}

void Cgi::generate_env (void) {

	add_env_variable("PATH_INFO", _path_info);
	add_env_variable("PATH_QUERY", _path_query);
	add_env_variable("CONTENT_LENGTH", ft_to_string(_body.size()));
	add_env_variable("SERVER_PROTOCOL", HTML_VERSION);
	add_env_variable("REQUEST_METHOD", (_request.getMethod() == GET) ? "GET" : "POST");
	_env.push_back(NULL);
}

void Cgi::add_env_variable (const std::string &name, const std::string &value) {
	_env.push_back(strdup((name + "=" + value).c_str()));
}

void Cgi::clean (void) {
	if (_save_std_out >= 0) {
 		dup2(_save_std_out, STDOUT_FILENO);
		close_and_change_value(_save_std_out);
	}
	if (_save_std_in >= 0) {
 		dup2(_save_std_in, STDIN_FILENO);
		close_and_change_value(_save_std_in);
	}
	close_and_change_value(_pipe_fd[READ_PIPE]);
	close_and_change_value(_pipe_fd[WRITE_PIPE]);
	close_and_change_value(_pipe_fd2[READ_PIPE]);
	close_and_change_value(_pipe_fd2[WRITE_PIPE]);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */