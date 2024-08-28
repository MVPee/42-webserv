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
    file.close();
    // while ((text.find("server") != std::string::npos) = position)
    // {
    //     // Continue le parsing à partir de position
    // }

    // std::cout << text << std::endl;
    Server test(text);
	std::cout << test << std::endl;
    try {
        test.mySocket();
        test.myBind();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }

    while(1) {
        try {
            test.myListen();
            test.myAccept();
            test.myReceive();
            test.mySend();
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return (1);
        }
    }

    return (0);
}