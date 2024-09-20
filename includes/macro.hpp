#ifndef MACRO_HPP
#define MACRO_HPP

# include <iostream>
# include <cstring>
# include <string>
# include <vector>
# include <sstream>
# include <map>
# include <cstdlib>
# include <algorithm>
# include <unistd.h>
# include <fstream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <poll.h>
# include <list>
# include <fcntl.h>
# include <filesystem>
# include <pthread.h>
# include <signal.h>
# include <dirent.h>
# include <sys/wait.h>
# include <ctime>

extern volatile bool stopRequested;

# define TIME_OUT 5
# define TIME_OUT_CGI 3.0
# define MAX_CLIENT 100
# define MAX_SERVER 5
# define GET 0
# define POST 1
# define DELETE 2

# define HEADER_DELIMITER "\r\n\r\n"
# define HEADER_SIZE		4

# define OFF 0
# define ON 1

# define SOCKET 0
# define BIND 1
# define LISTEN 2
# define ACCEPT 3
# define BUFFER_SIZE 2048

#define WRITE_PIPE 1
#define READ_PIPE 0

# define HTML_VERSION "HTTP/1.1"

# define OK							200
# define NO_CONTENT					204

# define REDIRECTION_PERMANENTLY    301

# define BAD_REQUEST				400
# define FORBIDDEN                  403
# define ERROR_NOT_FOUND			404
# define ERROR_REQUEST_TIMEOUT 		408
# define PAYLOAD_TOO_LARGE			413
# define UNSUPPORTED_MEDIA_TYPE		415
# define CLIENT_CLOSED_REQUEST		499

# define ERROR_INTERNAL_SERVER		500
# define NOT_IMPLEMENTED			501
# define HTTP_VERSION_NOT_SUPPORTED	505

enum ClientState {
    ReceivingHeader,
    HandlingBody,
    Completed,
    Error
};

size_t find_cgi_extension(const std::string &path);

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "methods/Post.hpp"
# include "methods/Get.hpp"
# include "methods/Cgi.hpp"
# include "Client.hpp"
# include "Cookie.hpp"

enum extension {HTML = 1, PHP = 2, ICO = 3};

# define R "\x1b[1;31m"
# define G "\x1b[1;32m"
# define B "\x1b[1;36m"
# define Y "\x1b[1;33m"
# define M "\x1b[1;35m"
# define C "\x1b[0m"

# define U "\x1b[4m"
# define BOLD "\x1b[1m"
# define ITALIC "\x1b[3m"
# define BLINK "\x1b[5m"

#endif