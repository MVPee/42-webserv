# include "includes/macro.hpp"

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
    if (file.is_open())
        file.close();
    // while ((text.find("server") != std::string::npos) = position)
    // {
    //     // Continue le parsing à partir de position
    // }

    // std::cout << text << std::endl;
    Server test(text);
	std::cout << test << std::endl; //* DEBUG
    try {
        test.mySocket();
        test.myBind();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }

    test.myListen();
    // while(1) {
        try {
            test.process();
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return (1);
        }
    // }

    return (0);
}