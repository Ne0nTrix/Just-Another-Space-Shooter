#include "WebServer.h"
#include <iostream>
#include <thread>
#include <sstream>
#include <chrono>
#include <httplib.h>

WebServer::WebServer(Scoreboard* sb, int serverPort) 
    : scoreboard(sb), port(serverPort), running(false) {
    
    server = new httplib::Server();
    server->Get("/", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content(generateHTML(), "text/html; charset=utf-8");
    });
    server->Get("/api/scores", [this](const httplib::Request&, httplib::Response& res) {
        scoreboard->loadScores();
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_content(generateJSON(), "application/json; charset=utf-8");
    });
    server->Get("/style.css", [](const httplib::Request&, httplib::Response& res) {
        std::string css = R"(
            body {
                font-family: 'Arial', sans-serif;
                background: linear-gradient(135deg, #1e3c72, #2a5298);
                margin: 0;
                padding: 20px;
                color: white;
                min-height: 100vh;
            }
            .container {
                max-width: 800px;
                margin: 0 auto;
                background: rgba(0,0,0,0.3);
                padding: 30px;
                border-radius: 15px;
                box-shadow: 0 8px 32px rgba(0,0,0,0.3);
                backdrop-filter: blur(10px);
            }
            h1 {
                text-align: center;
                color: #00ff88;
                font-size: 3em;
                margin-bottom: 30px;
                text-shadow: 2px 2px 4px rgba(0,0,0,0.5);
                animation: glow 2s ease-in-out infinite alternate;
            }
            @keyframes glow {
                from { text-shadow: 2px 2px 4px rgba(0,0,0,0.5), 0 0 10px #00ff88; }
                to { text-shadow: 2px 2px 4px rgba(0,0,0,0.5), 0 0 20px #00ff88, 0 0 30px #00ff88; }
            }
            table {
                width: 100%;
                border-collapse: collapse;
                margin-top: 20px;
                background: rgba(255,255,255,0.05);
                border-radius: 10px;
                overflow: hidden;
            }
            th, td {
                padding: 15px;
                text-align: left;
                border-bottom: 1px solid rgba(255,255,255,0.2);
            }
            th {
                background: rgba(0,255,136,0.2);
                color: #00ff88;
                font-weight: bold;
                text-transform: uppercase;
                text-shadow: 1px 1px 2px rgba(0,0,0,0.5);
            }
            tr:hover {
                background: rgba(255,255,255,0.1);
                transition: background-color 0.3s ease;
            }
            .rank {
                font-weight: bold;
                color: #ffd700;
                text-shadow: 1px 1px 2px rgba(0,0,0,0.5);
            }
            .difficulty-easy { 
                color: #90EE90; 
                font-weight: bold;
            }
            .difficulty-medium { 
                color: #FFD700; 
                font-weight: bold;
            }
            .difficulty-hard { 
                color: #FF6B6B; 
                font-weight: bold;
            }
            .refresh-btn {
                background: linear-gradient(45deg, #00ff88, #00cc6a);
                color: black;
                border: none;
                padding: 12px 25px;
                border-radius: 25px;
                cursor: pointer;
                font-weight: bold;
                margin: 20px auto;
                display: block;
                font-size: 16px;
                transition: all 0.3s ease;
                box-shadow: 0 4px 15px rgba(0,255,136,0.3);
            }
            .refresh-btn:hover {
                background: linear-gradient(45deg, #00cc6a, #00aa55);
                transform: translateY(-2px);
                box-shadow: 0 6px 20px rgba(0,255,136,0.4);
            }
            .no-scores {
                text-align: center;
                color: #888;
                font-style: italic;
                padding: 40px;
                font-size: 18px;
            }
            .last-updated {
                text-align: center;
                color: #ccc;
                font-size: 12px;
                margin-top: 20px;
            }
        )";
        res.set_content(css, "text/css");
    });
    server->Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\",\"service\":\"Space Shooter Highscores\"}", "application/json");
    });
}

WebServer::~WebServer() {
    stop();
    if (server) {
        delete server;
        server = nullptr;
    }
}

std::string WebServer::generateHTML() {
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"en\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>Space Shooter - High Scores</title>\n";
    html << "    <link rel=\"stylesheet\" href=\"/style.css\">\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <div class=\"container\">\n";
    html << "        <h1>🚀 Space Shooter High Scores 🚀</h1>\n";
    html << "        <button class=\"refresh-btn\" onclick=\"refreshScores()\">🔄 Refresh Scores</button>\n";
    html << "        <table id=\"scoresTable\">\n";
    html << "            <thead>\n";
    html << "                <tr>\n";
    html << "                    <th>🏆 Rank</th>\n";
    html << "                    <th>👤 Player Name</th>\n";
    html << "                    <th>⭐ Score</th>\n";
    html << "                    <th>🎯 Difficulty</th>\n";
    html << "                </tr>\n";
    html << "            </thead>\n";
    html << "            <tbody>\n";

    const auto& scores = scoreboard->getScores();

    if (scores.empty()) {
        html << "                <tr>\n";
        html << "                    <td colspan=\"4\" class=\"no-scores\">\n";
        html << "                        🎮 No scores yet! Be the first to play! 🎮\n";
        html << "                    </td>\n";
        html << "                </tr>\n";
    } else {
        for (size_t i = 0; i < scores.size(); i++) {
            std::string difficultyClass;
            std::string difficultyText;
            std::string difficultyIcon;

            switch (scores[i].difficulty) {
            case Difficulty::EASY:
                difficultyClass = "difficulty-easy";
                difficultyText = "EASY";
                difficultyIcon = "🟢";
                break;
            case Difficulty::MEDIUM:
                difficultyClass = "difficulty-medium";
                difficultyText = "MEDIUM";
                difficultyIcon = "🟡";
                break;
            case Difficulty::HARD:
                difficultyClass = "difficulty-hard";
                difficultyText = "HARD";
                difficultyIcon = "🔴";
                break;
            }

            std::string rankIcon;
            if (i == 0) rankIcon = "🥇";
            else if (i == 1) rankIcon = "🥈";
            else if (i == 2) rankIcon = "🥉";
            else rankIcon = "";

            html << "                <tr>\n";
            html << "                    <td class=\"rank\">" << rankIcon << " " << (i + 1) << "</td>\n";
            html << "                    <td>" << scores[i].name << "</td>\n";
            html << "                    <td>" << scores[i].score << "</td>\n";
            html << "                    <td class=\"" << difficultyClass << "\">" << difficultyIcon << " " << difficultyText << "</td>\n";
            html << "                </tr>\n";
        }
    }

    html << "            </tbody>\n";
    html << "        </table>\n";
    html << "        <div class=\"last-updated\" id=\"lastUpdated\">\n";
    html << "            Last updated: <span id=\"updateTime\"></span>\n";
    html << "        </div>\n";
    html << "    </div>\n";
    html << "\n";
    html << "    <script>\n";
    html << "        function updateTime() {\n";
    html << "            const now = new Date();\n";
    html << "            document.getElementById('updateTime').textContent = now.toLocaleString();\n";
    html << "        }\n";
    html << "\n";
    html << "        function refreshScores() {\n";
    html << "            const button = document.querySelector('.refresh-btn');\n";
    html << "            button.textContent = '🔄 Refreshing...';\n";
    html << "            button.disabled = true;\n";
    html << "\n";
    html << "            fetch('/api/scores')\n";
    html << "                .then(response => response.json())\n";
    html << "                .then(data => {\n";
    html << "                    updateTable(data);\n";
    html << "                    updateTime();\n";
    html << "                    button.textContent = '🔄 Refresh Scores';\n";
    html << "                    button.disabled = false;\n";
    html << "                })\n";
    html << "                .catch(error => {\n";
    html << "                    console.error('Error:', error);\n";
    html << "                    button.textContent = '❌ Error - Try Again';\n";
    html << "                    button.disabled = false;\n";
    html << "                    setTimeout(() => {\n";
    html << "                        button.textContent = '🔄 Refresh Scores';\n";
    html << "                    }, 2000);\n";
    html << "                });\n";
    html << "        }\n";
    html << "\n";
    html << "        function updateTable(scores) {\n";
    html << "            const tbody = document.querySelector('#scoresTable tbody');\n";
    html << "            tbody.innerHTML = '';\n";
    html << "\n";
    html << "            if (scores.length === 0) {\n";
    html << "                tbody.innerHTML = '<tr><td colspan=\"4\" class=\"no-scores\">🎮 No scores yet! Be the first to play! 🎮</td></tr>';\n";
    html << "                return;\n";
    html << "            }\n";
    html << "\n";
    html << "            scores.forEach((score, index) => {\n";
    html << "                const row = document.createElement('tr');\n";
    html << "                const difficultyClass = 'difficulty-' + score.difficulty.toLowerCase();\n";
    html << "\n";
    html << "                let difficultyIcon = '';\n";
    html << "                switch(score.difficulty) {\n";
    html << "                    case 'EASY': difficultyIcon = '🟢'; break;\n";
    html << "                    case 'MEDIUM': difficultyIcon = '🟡'; break;\n";
    html << "                    case 'HARD': difficultyIcon = '🔴'; break;\n";
    html << "                }\n";
    html << "\n";
    html << "                let rankIcon = '';\n";
    html << "                if (index === 0) rankIcon = '🥇';\n";
    html << "                else if (index === 1) rankIcon = '🥈';\n";
    html << "                else if (index === 2) rankIcon = '🥉';\n";
    html << "\n";
    html << "                row.innerHTML = '<td class=\"rank\">' + rankIcon + ' ' + (index + 1) + '</td>' +\n";
    html << "                              '<td>' + score.name + '</td>' +\n";
    html << "                              '<td>' + score.score + '</td>' +\n";
    html << "                              '<td class=\"' + difficultyClass + '\">' + difficultyIcon + ' ' + score.difficulty + '</td>';\n";
    html << "\n";
    html << "                tbody.appendChild(row);\n";
    html << "            });\n";
    html << "        }\n";
    html << "\n";
    html << "        setInterval(refreshScores, 30000);\n";
    html << "        updateTime();\n";
    html << "\n";
    html << "        document.addEventListener('DOMContentLoaded', function() {\n";
    html << "            console.log('🚀 Space Shooter Highscores loaded!');\n";
    html << "        });\n";
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string WebServer::generateJSON() {
    std::stringstream json;
    json << "[";
    
    const auto& scores = scoreboard->getScores();
    
    for (size_t i = 0; i < scores.size(); i++) {
        if (i > 0) json << ",";
        
        std::string difficultyText;
        switch (scores[i].difficulty) {
            case Difficulty::EASY: 
                difficultyText = "EASY"; 
                break;
            case Difficulty::MEDIUM: 
                difficultyText = "MEDIUM"; 
                break;
            case Difficulty::HARD: 
                difficultyText = "HARD"; 
                break;
        }
        
        json << "{";
        json << "\"rank\":" << (i + 1) << ",";
        json << "\"name\":\"" << scores[i].name << "\",";
        json << "\"score\":" << scores[i].score << ",";
        json << "\"difficulty\":\"" << difficultyText << "\"";
        json << "}";
    }
    
    json << "]";
    return json.str();
}

void WebServer::start() {
    if (running) return;
    
    running = true;
    std::cout << "🌐 Starting web server on http://localhost:" << port << std::endl;
    std::cout << "📊 Visit the URL above to view live highscores!" << std::endl;
    
    serverThread = std::thread([this]() {
        if (!server->listen("0.0.0.0", port)) {
            std::cerr << "❌ Failed to start web server on port " << port << std::endl;
            running = false;
        }
    });
}

void WebServer::stop() {
    if (!running) return;
    
    std::cout << "🛑 Stopping web server..." << std::endl;
    running = false;
    
    if (server) {
        server->stop();
    }
    
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

bool WebServer::isRunning() {
    return running;
}