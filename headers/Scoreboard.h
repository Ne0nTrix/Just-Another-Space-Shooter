#pragma once
#include "PlayerData.h"
#include <vector>
#include <string>

class Scoreboard {
private:
    std::vector<PlayerScore> scores;
    const int MAX_SCORES = 10;
    void* connection;
    const std::string DB_HOST = "localhost";
    const std::string DB_USER = "admin";
    const std::string DB_PASS = "2137";
    const std::string DB_NAME = "space_shooter";
    const int DB_PORT = 3306;
    
    bool connectToDatabase();
    void closeConnection();

public:
    Scoreboard();
    ~Scoreboard();
    
    void loadScores();
    
    bool addScore(const PlayerScore& score);
    const std::vector<PlayerScore>& getScores() const;
    
    void clearScores();
};