#pragma once
#include <string>

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

struct PlayerScore {
    std::string name;
    int score;
    Difficulty difficulty;
    
    PlayerScore(const std::string& n = "", int s = 0, Difficulty d = Difficulty::MEDIUM)
        : name(n), score(s), difficulty(d) {}
};

class PlayerData {
private:
    std::string playerName;
    int currentScore;
    Difficulty difficulty;

public:
    PlayerData();
    
    void setPlayerName(const std::string& name);
    const std::string& getPlayerName() const;
    
    void setDifficulty(Difficulty diff);
    Difficulty getDifficulty() const;
    
    void addScore(int points);
    void resetScore();
    int getScore() const;
    
    float getDifficultyMultiplier() const;
};