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
                            "let isClicked = false;\n"
                        "\n"
                        "// Update the number and create falling cookies on click\n"
                        "cookie.addEventListener('click', function () {\n"
                        "    if (!isClicked) {\n"
                        "        // Grow/shrink animation\n"
                        "        cookie.classList.add('grow');\n"
                        "        setTimeout(() => {\n"
                        "            cookie.classList.remove('grow');\n"
                        "            cookie.classList.add('shrink');\n"
                        "            setTimeout(() => {\n"
                        "                cookie.classList.remove('shrink');\n"
                        "            }, 300);\n"
                        "        }, 300);\n"
                        "\n"
                        "        // Increment the count\n"
                        "        number.textContent = count;\n"
                        "\n"
                        "        // Create and animate the falling cookie\n"
                        "        createFallingCookie();\n"
                        "\n"
                        "        // Prevent multiple clicks until the animation is done\n"
                        "        isClicked = true;\n"
                        "        setTimeout(() => {\n"
                        "            isClicked = false;\n"
                        "        }, 100);\n"
                        "    }\n"
                        "});\n"
                        "\n"
                        "function createFallingCookie() {\n"
                        "    // Create a new cookie element\n"
                        "    const fallingCookie = document.createElement('div');\n"
                        "    fallingCookie.classList.add('falling-cookie');\n"
                        "\n"
                        "    // Set a random horizontal position for the cookie\n"
                        "    const randomX = Math.floor(Math.random() * window.innerWidth);\n"
                        "    fallingCookie.style.left = randomX + 'px';\n"
                        "\n"
                        "    // Append the falling cookie to the body\n"
                        "    document.body.appendChild(fallingCookie);\n"
                        "\n"
                        "    // Remove the cookie after the animation ends\n"
                        "    setTimeout(() => {\n"
                        "        fallingCookie.remove();\n"
                        "    }, 3000);\n"
                        "}\n"
                        "</script>\n";
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