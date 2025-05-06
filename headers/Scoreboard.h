#pragma once
#include "PlayerData.h"
#include <vector>
#include <string>

class Scoreboard {
private:
    std::vector<PlayerScore> scores;
    const std::string SCORE_FILE = "highscores.dat";
    const int MAX_SCORES = 10;

public:
    Scoreboard();
    ~Scoreboard();
    
    void loadScores();
    void saveScores();
    
    bool addScore(const PlayerScore& score);
    const std::vector<PlayerScore>& getScores() const;
    
    void clearScores();
};