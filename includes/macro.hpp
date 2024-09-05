#ifndef MACRO_HPP
#define MACRO_HPP

# include <iostream>
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


# define GET 0
# define POST 1
# define DELETE 2

# define OFF 0
# define ON 1

# define SOCKET 0
# define BIND 1
# define LISTEN 2
# define ACCEPT 3
# define BUFFER_SIZE 2056

# define OK						200

# define BAD_REQUEST			400
# define PAYLOAD_TOO_LARGE		413
# define CLIENT_CLOSED_REQUEST	499

# define INTERNAL_SERVER_ERROR	500
# define NOT_IMPLEMENTED		501

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