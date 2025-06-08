#include "WebServer.h"
#include "Scoreboard.h"
#include <iostream>
#include <signal.h>

WebServer* globalServer = nullptr;

void signalHandler(int signum) {
    std::cout << "\nShutting down server..." << std::endl;
    if (globalServer) {
        globalServer->stop();
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);
    
    Scoreboard scoreboard;
    WebServer server(&scoreboard, 8080);
    globalServer = &server;
    
    std::cout << "Starting Space Shooter Highscore Server..." << std::endl;
    std::cout << "Visit http://localhost:8080 to view highscores" << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    server.start();
    
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}