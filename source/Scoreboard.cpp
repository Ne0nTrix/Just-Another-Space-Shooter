#include "Scoreboard.h"
#include <iostream>
#include <algorithm>
#include <mysql/mysql.h>

Scoreboard::Scoreboard() {
    connection = nullptr;
    try {
        if (connectToDatabase()) {
            loadScores();
        }
    }
    catch (...) {
        std::cerr << "Exception during MySQL initialization." << std::endl;
    }
}

Scoreboard::~Scoreboard() {
    closeConnection();
    mysql_library_end();
}

bool Scoreboard::connectToDatabase() {
    try {
        if (connection != nullptr) {
            closeConnection();
        }
        
        MYSQL* conn = mysql_init(NULL);
        if (conn == nullptr) {
            return false;
        }
        
        unsigned int timeout = 5;
        mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        
        if (!mysql_real_connect(conn, DB_HOST.c_str(), DB_USER.c_str(), 
                              DB_PASS.c_str(), DB_NAME.c_str(), DB_PORT, NULL, 0)) {
            std::cerr << "MySQL connection failed: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return false;
        }
        
        connection = static_cast<void*>(conn);
        return true;
    }
    catch (...) {
        return false;
    }
}

void Scoreboard::closeConnection() {
    if (connection) {
        mysql_close(static_cast<MYSQL*>(connection));
        connection = nullptr;
    }
}

void Scoreboard::loadScores() {
    scores.clear();
    
    try {
        MYSQL* conn = static_cast<MYSQL*>(connection);
        const char* query = "SELECT player_name, score, difficulty FROM highscores ORDER BY score DESC LIMIT 10";
        
        if (mysql_query(conn, query) != 0) {
            std::cerr << "Query failed: " << mysql_error(conn) << std::endl;
            return;
        }
        
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr) {
            std::cerr << "Failed to store result: " << mysql_error(conn) << std::endl;
            return;
        }
        
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            std::string name = row[0];
            int score = std::stoi(row[1]);
            std::string diffStr = row[2];
            
            Difficulty diff;
            if (diffStr == "EASY") diff = Difficulty::EASY;
            else if (diffStr == "MEDIUM") diff = Difficulty::MEDIUM;
            else diff = Difficulty::HARD;
            
            scores.emplace_back(name, score, diff);
        }
        
        mysql_free_result(result);
        std::cout << "Loaded " << scores.size() << " scores from database" << std::endl;
    }
    catch (...) {
        std::cerr << "Exception in loadScores." << std::endl;
    }
}

bool Scoreboard::addScore(const PlayerScore& score) {
    for (const auto& existingScore : scores) {
        if (existingScore.name == score.name && 
            existingScore.score == score.score && 
            existingScore.difficulty == score.difficulty) {
            return false;
        }
    }
    
    scores.push_back(score);
    
    std::sort(scores.begin(), scores.end(), [](const PlayerScore& a, const PlayerScore& b) {
        return a.score > b.score;
    });
    
    if (scores.size() > MAX_SCORES) {
        scores.resize(MAX_SCORES);
    }
    
    bool dbSuccess = false;
    
    if (connection != nullptr) {
        try {
            MYSQL* conn = static_cast<MYSQL*>(connection);
            
            const char* diffStr;
            switch (score.difficulty) {
                case Difficulty::EASY: diffStr = "EASY"; break;
                case Difficulty::MEDIUM: diffStr = "MEDIUM"; break;
                case Difficulty::HARD: diffStr = "HARD"; break;
                default: diffStr = "MEDIUM"; break;
            }
            
            char escapedName[101];
            mysql_real_escape_string(conn, escapedName, score.name.c_str(), 
                                  static_cast<unsigned long>(score.name.length()));
            
            char query[256];
            snprintf(query, sizeof(query), 
                   "INSERT INTO highscores (player_name, score, difficulty) VALUES ('%s', %d, '%s')",
                   escapedName, score.score, diffStr);
            
            if (mysql_query(conn, query) == 0) {
                dbSuccess = true;
                std::cout << "Score saved to database successfully" << std::endl;
            } else {
                std::cerr << "Failed to save to database: " << mysql_error(conn) << std::endl;
            }
        }
        catch (...) {
            std::cerr << "Exception while saving to database" << std::endl;
        }
    }
    
    
    return true;
}

const std::vector<PlayerScore>& Scoreboard::getScores() const {
    return scores;
}

void Scoreboard::clearScores() {
    scores.clear();
    
    if (connection != nullptr) {
        try {
            MYSQL* conn = static_cast<MYSQL*>(connection);
            const char* query = "TRUNCATE TABLE highscores";
            if (mysql_query(conn, query) != 0) {
                std::cerr << "Failed to clear database: " << mysql_error(conn) << std::endl;
            }
        }
        catch (...) {
            std::cerr << "Exception while clearing database" << std::endl;
        }
    }
    
}