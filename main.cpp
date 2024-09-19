# include "includes/macro.hpp"

volatile bool stopRequested = false;

void handleSignal(int signal) {
    std::cout << "\n";
    if (signal == SIGINT || signal == SIGQUIT)
        stopRequested = true;
}

static void* serverThread(void* arg) {
    Server* server = static_cast<Server*>(arg);
    try {
        server->mySocket();
        server->myBind();
        server->myListen();
        while (!stopRequested)
            server->process();
    }
    catch (std::exception &e) {
        server->message(e.what(), R);
    }
    return NULL;
}

int main(int ac, char **av) {
    std::string line;
    std::string text;

    if (ac != 2)
        return 1;

    std::ifstream file(av[1]);
    if (!file.is_open() && !file.good())
        std::cerr << "couldn't open config file..." << std::endl;
    while (std::getline(file, line))
        text.append(line + "\n");
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

    std::vector<pthread_t> server_threads;
    for (size_t i = 0; i < servers.size(); i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, serverThread, static_cast<void*>(servers[i]));
        server_threads.push_back(thread);
    }
    
    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    signal(SIGPIPE, SIG_IGN); // A retirer (verif si SIGPIPE with siege on linux at 19)

    for (size_t i = 0; i < server_threads.size(); i++)
        pthread_join(server_threads[i], NULL);

    for (size_t i = 0; i < servers.size(); i++)
        delete servers.at(i);
}