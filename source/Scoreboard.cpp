#include "Scoreboard.h"
#include <fstream>
#include <algorithm>
#include <iostream>

Scoreboard::Scoreboard() {
    loadScores();
}

Scoreboard::~Scoreboard() {
    saveScores();
}

void Scoreboard::loadScores() {
    std::ifstream file(SCORE_FILE, std::ios::binary);
    
    if (!file.is_open()) {
        return; // File doesn't exist, start with empty scores
    }
    
    try {
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        
        scores.clear();
        for (size_t i = 0; i < size && i < MAX_SCORES; i++) {
            PlayerScore score;
            
            // Read name length
            size_t nameLength;
            file.read(reinterpret_cast<char*>(&nameLength), sizeof(size_t));
            
            // Read name
            char* nameBuffer = new char[nameLength + 1];
            file.read(nameBuffer, nameLength);
            nameBuffer[nameLength] = '\0';
            score.name = nameBuffer;
            delete[] nameBuffer;
            
            // Read score and difficulty
            file.read(reinterpret_cast<char*>(&score.score), sizeof(int));
            file.read(reinterpret_cast<char*>(&score.difficulty), sizeof(Difficulty));
            
            scores.push_back(score);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading scores: " << e.what() << std::endl;
        scores.clear();
    }
    
    file.close();
}

void Scoreboard::saveScores() {
    std::ofstream file(SCORE_FILE, std::ios::binary | std::ios::trunc);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for saving scores." << std::endl;
        return;
    }
    
    try {
        size_t size = scores.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        
        for (const auto& score : scores) {
            // Write name length
            size_t nameLength = score.name.length();
            file.write(reinterpret_cast<const char*>(&nameLength), sizeof(size_t));
            
            // Write name
            file.write(score.name.c_str(), nameLength);
            
            // Write score and difficulty
            file.write(reinterpret_cast<const char*>(&score.score), sizeof(int));
            file.write(reinterpret_cast<const char*>(&score.difficulty), sizeof(Difficulty));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving scores: " << e.what() << std::endl;
    }
    
    file.close();
}

bool Scoreboard::addScore(const PlayerScore& score) {
    // Check if this exact same score already exists
    for (const auto& existingScore : scores) {
        // If there's already an identical entry, don't add a duplicate
        if (existingScore.name == score.name && 
            existingScore.score == score.score && 
            existingScore.difficulty == score.difficulty) {
            return false; // Score already exists, don't add it again
        }
    }
    
    // No duplicate found, add the new score
    scores.push_back(score);
    
    // Sort scores in descending order
    std::sort(scores.begin(), scores.end(), [](const PlayerScore& a, const PlayerScore& b) {
        return a.score > b.score;
    });
    
    // Keep only top MAX_SCORES
    if (scores.size() > MAX_SCORES) {
        scores.resize(MAX_SCORES);
    }
    
    return true; // Score was added
}

const std::vector<PlayerScore>& Scoreboard::getScores() const {
    return scores;
}

void Scoreboard::clearScores() {
    scores.clear();
}