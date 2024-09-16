#include "../includes/Cookie.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cookie::Cookie(const std::string &request, std::string &response_header) : _id(0) {
    std::string temp = find_value_by_id("id", request);

    if (!temp.empty())
        _id = atoi(temp.c_str()) - 1;

    if (request.find(" /cookie ") != std::string::npos ||
        request.find(" /cookie/ ") != std::string::npos ||
        request.find(" /cookie/cookie ") != std::string::npos ||
        request.find(" /cookie/cookie.html ") != std::string::npos) {
        _id++;

        response_header += "<script>\n"
                            "const cookie = document.getElementById('cookie');\n"
                            "const number = document.getElementById('number');\n"
                            "let count = " + ft_to_string(_id) + ";\n"
                            "cookie.addEventListener('click', () => {\n"
                            "\tcount++;\n"
                            "\tdocument.cookie = 'id=' + count + '; Path=/;';\n"
                            "\tnumber.textContent = count;\n"
                            "});\n"
                            "document.querySelector('.number').textContent = '" + ft_to_string(_id) + "';\n"
                            "</script>";
    }
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cookie::~Cookie() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

std::string Cookie::find_value_by_id(const std::string &id, const std::string &request) {
    std::string::size_type start = request.find("Cookie: ");
    if (start != std::string::npos) {
        std::string::size_type end = request.find("\r\n", start);
        std::string line = request.substr(start, end - start);
        start = line.find(id + "=");
        if (start != std::string::npos) {
            end = line.find(";", start);
            if (end == std::string::npos)
                end = line.size();
            std::string value = line.substr(start + id.size() + 1, end - (start + id.size() + 1));
            return value;
        }
    }
    return "";
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */