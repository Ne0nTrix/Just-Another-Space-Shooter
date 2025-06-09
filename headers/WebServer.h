#pragma once
#include "Scoreboard.h"
#include <string>
#include <thread>
#include <atomic>

namespace httplib {
    class Server;
}

class WebServer {
private:
    httplib::Server* server;
    Scoreboard* scoreboard;
    int port;
    std::thread serverThread;
    std::atomic<bool> running;
    
    std::string generateHTML();
    std::string generateJSON();
    
public:
    WebServer(Scoreboard* sb, int serverPort = 8080);
    ~WebServer();
    void start();
    void stop();
    bool isRunning();
};