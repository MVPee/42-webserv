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

extern volatile bool stopRequested;

# define TIME_OUT 10
# define MAX_CLIENT 100
# define GET 0
# define POST 1
# define DELETE 2

# define OFF 0
# define ON 1

# define SOCKET 0
# define BIND 1
# define LISTEN 2
# define ACCEPT 3
# define BUFFER_SIZE 2048

# define HTML_VERSION "HTTP/1.1"

# define OK							200
# define NO_CONTENT					204

# define REDIRECTION_PERMANENTLY    301

# define BAD_REQUEST				400
# define FORBIDDEN                  403
# define ERROR_NOT_FOUND			404
# define ERROR_REQUEST_TIMEOUT 		408
# define PAYLOAD_TOO_LARGE			413
# define CLIENT_CLOSED_REQUEST		499

# define ERROR_INTERNAL_SERVER		500
# define NOT_IMPLEMENTED			501
# define HTTP_VERSION_NOT_SUPPORTED	505

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "methods/Post.hpp"
# include "methods/Get.hpp"

enum extension {HTML = 1, PHP = 2, ICO = 3};

# define R "\x1b[1;31m"
# define G "\x1b[1;32m"
# define B "\x1b[1;36m"
# define Y "\x1b[1;33m"
# define M "\x1b[1;35m"
# define C "\x1b[0m"

std::string get_data_in_header(std::string &header, std::string first_delimiter, std::string end_delimiter);

#endif