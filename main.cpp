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

    std::vector<Server *> servers;
    size_t start = 0;
    int i = 0;
    while ((start = text.find('{', start)) != std::string::npos) {
        size_t end = text.find('}', start);
        if (end == std::string::npos) {
            std::cerr << "Not a valid conf file.." << std::endl;
            return(1);
        }
        std::string config = text.substr(start + 1, end - start - 1);
        Server *newServer = new Server(config);
        servers.push_back(newServer);
        start = end + 1;
        i++;
    }

    for (size_t i = 0; i < servers.size(); i++) {
        // std::cout << test << std::endl; //* DEBUG
        try {
            servers.at(i)->mySocket();
            servers.at(i)->myBind();
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return (1);
        }
        servers.at(i)->myListen();
    }
    while(1) {
        for (size_t i = 0; i < servers.size(); i++) {
            try {
                servers.at(i)->process();
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                return (1);
            }
        }
    }
    return (0);
}