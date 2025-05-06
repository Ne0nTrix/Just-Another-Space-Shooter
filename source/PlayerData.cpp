#include "PlayerData.h"

PlayerData::PlayerData()
    : playerName(""), currentScore(0), difficulty(Difficulty::MEDIUM)
{
}

void PlayerData::setPlayerName(const std::string& name) {
    playerName = name;
}

const std::string& PlayerData::getPlayerName() const {
    return playerName;
}

void PlayerData::setDifficulty(Difficulty diff) {
    difficulty = diff;
}

Difficulty PlayerData::getDifficulty() const {
    return difficulty;
}

void PlayerData::addScore(int points) {
    currentScore += points * getDifficultyMultiplier();
}

void PlayerData::resetScore() {
    currentScore = 0;
}

int PlayerData::getScore() const {
    return currentScore;
}

float PlayerData::getDifficultyMultiplier() const {
    switch (difficulty) {
        case Difficulty::EASY:
            return 1.0f;
        case Difficulty::MEDIUM:
            return 1.5f;
        case Difficulty::HARD:
            return 2.0f;
        default:
            return 1.0f;
    }
}