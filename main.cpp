# include <iostream>
# include <fstream>
# include <string>
# include "includes/Server.hpp"
# include <cstring>

int main(int ac, char **av) {
    std::string line;
    std::string text;
    

    if (ac != 2)
        return (1);

    std::ifstream file(av[1]);
    if (!file.is_open() && !file.good())
        std::cerr << "couldn't open config file..." << std::endl;
    while (std::getline(file, line)) {
        text.append(line + "\n");
    }
    // while ((text.find("server") != std::string::npos) = position)
    // {
    //     // Continue le parsing à partir de position
    // }

    // std::cout << text << std::endl;
    Server test(text);
    file.close();

    return (0);
}