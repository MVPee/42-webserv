# include <iostream>
# include <fstream>
# include <string>

int main(int ac, char **av) {
    std::string line;
    std::string text;

    if (ac != 2)
        return (1);

    std::ifstream file(av[1]);
    if (!file.is_open() && !file.good())
        std::cerr << "couldn't open config file..." << std::endl;
    while (std::getline(file, line))
        text.append(line + '\n');

    std::cout << text << std::endl;

    return (0);
}