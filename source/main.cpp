#include "raylib.h"
#include "Background.h"
#include "Player.h"
#include "Bullet.h"
#include "Obstacle.h"
#include "GameStateManager.h"
#include "ComplexEnemy.h"
#include "PlayerData.h"
#include "Scoreboard.h"
#include "Upgrade.h"
#include "LaserTurret.h"
#include "HeavyEnemy.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

bool canSpawnEnemies = false;
const float UPGRADE_DROP_CHANCE = 15.0f;
const float LIFE_DROP_CHANCE = 20.0f;

bool CheckCollision(Vector2 pos1, float radius1, Vector2 pos2, float radius2) {
    float distance = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
    return distance < (radius1 + radius2);
}

void ShowLevelTransitionScreen(const std::string& levelText, int width, int height) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(levelText.c_str(), width / 2 - MeasureText(levelText.c_str(), 40) / 2, height / 2 - 20, 40, WHITE);
    EndDrawing();
    float startTime = GetTime();
    while (GetTime() - startTime < 2.0f) {}
    canSpawnEnemies = true;
}

void CreateMeteor(std::vector<Obstacle>& meteors, Texture* smallTexture, Texture* mediumTexture, Texture* largeTexture,
    int sizeChoice, float posX, float posY, float playerX, float playerY, float safeDistance) {
    do {
        posX = static_cast<float>(GetRandomValue(0, GetScreenWidth()));
        posY = static_cast<float>(GetRandomValue(0, GetScreenHeight()));

        float deltaX = posX - playerX;
        float deltaY = posY - playerY;
        float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

        if (distance >= safeDistance) break;

    } while (true);

    if (sizeChoice == 0) {
        meteors.emplace_back(smallTexture, 8);
    }
    else if (sizeChoice == 1) {
        meteors.emplace_back(mediumTexture, 16);
    }
    else {
        meteors.emplace_back(largeTexture, 32);
    }

    meteors.back().posx = posX;
    meteors.back().posy = posY;
}

void ResetGameState(Player& player, std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, 
                   std::vector<ComplexEnemy>& complexEnemies, std::vector<Bullet>& enemyBullets,
                   std::vector<Obstacle>& meteors, std::vector<Upgrade>& upgrades, std::vector<LaserTurret>& laserTurrets, std::vector<HeavyEnemy>& heavyEnemies,
                   PlayerData& playerData, float initialPosX, float initialPosY) {
    bullets.clear();
    enemies.clear();
    complexEnemies.clear();
    enemyBullets.clear();
    meteors.clear();
    upgrades.clear();
    laserTurrets.clear();
    heavyEnemies.clear();
    
    player.ResetAllUpgrades();
    player.SetHealth(5);
    player.posx = initialPosX;
    player.posy = initialPosY;
    
    playerData.resetScore();
}

void CreateUpgradeDrop(std::vector<Upgrade>& upgrades, Texture* upgradeTexture, float posX, float posY) {
    float totalChance = UPGRADE_DROP_CHANCE + LIFE_DROP_CHANCE;
    float roll = GetRandomValue(0, 1000) / 10.0f;
    
    if (roll < totalChance) {
        UpgradeType type;
        if (roll < LIFE_DROP_CHANCE) {
            type = UpgradeType::EXTRA_LIFE;
        } else {
            int upgradeChoice = GetRandomValue(0, 6);
            switch (upgradeChoice) {
                case 0: type = UpgradeType::FASTER_SHOOTING; break;
                case 1: type = UpgradeType::BIGGER_BULLETS; break;
                case 2: type = UpgradeType::TRIPLE_SHOT; break;
                case 3: type = UpgradeType::PIERCING_BULLETS; break;
                case 4: type = UpgradeType::SHIELD; break;
                case 5: type = UpgradeType::FASTER_MOVEMENT; break;
                case 6: type = UpgradeType::SLOW_ENEMIES; break;
                default: type = UpgradeType::FASTER_SHOOTING; break;
            }
        }
        
        upgrades.emplace_back(upgradeTexture, posX, posY, type);
    }
}

int main() {
    const int WIDTH = 1080;
    const int HEIGHT = 960;
    const float initialPosX = WIDTH / 2;
    const float initialPosY = HEIGHT - 64;

    InitWindow(WIDTH, HEIGHT, "Just Another Space Invaders");
    SetTargetFPS(60);
    InitAudioDevice();

    GameStateManager gameStateManager(GameState::MAIN_MENU);

    Texture backgroundTex = LoadTexture("assets/Space_Background.png");
    Texture shipTexture = LoadTexture("assets/ship_texture.png");
    Texture bulletTexture = LoadTexture("assets/bullet_texture.png");
    Texture smallMeteorTexture = LoadTexture("assets/meteor_small_texture.png");
    Texture mediumMeteorTexture = LoadTexture("assets/meteor_medium_texture.png");
    Texture largeMeteorTexture = LoadTexture("assets/meteor_big_texture.png");
    Texture enemyTexture = LoadTexture("assets/enemy_texture.png");
    Texture enemyBulletTexture = LoadTexture("assets/enemy_bullet_texture.png");
    Texture healthTexture = LoadTexture("assets/health1_texture.png");
    Texture upgradeTexture = LoadTexture("assets/upgrade_texture.png");
    Texture laserTexture = LoadTexture("assets/laser_texture.png");
    
    Sound backgroundMusic = LoadSound("assets/music/background.wav");
    Sound blipSelectSound = LoadSound("assets/music/blipSelect.wav");
    Sound hitEnemySound = LoadSound("assets/music/hitEnemy.wav");
    Sound hitHurtSound = LoadSound("assets/music/hitHurt.wav");
    Sound laserShootSound = LoadSound("assets/music/laserShoot.wav");
    Sound powerUpSound = LoadSound("assets/music/powerUp.wav");
    Sound enemyShootSound = LoadSound("assets/music/laserShootEnemy.wav");
    Sound laserBeamSound = LoadSound("assets/music/laser.wav");
    Sound heavyHitSound = LoadSound("assets/music/heavy.wav");


    SetSoundVolume(backgroundMusic, 0.6f);
    SetSoundVolume(blipSelectSound, 0.7f);
    SetSoundVolume(hitEnemySound, 0.6f);
    SetSoundVolume(hitHurtSound, 0.8f);
    SetSoundVolume(laserShootSound, 0.5f);
    SetSoundVolume(powerUpSound, 0.7f);
    SetSoundVolume(laserBeamSound, 0.4f);
    SetSoundVolume(heavyHitSound, 0.6f);  
    
    PlaySound(backgroundMusic);
    bool musicPlaying = true;

    Background bg(&backgroundTex, 0, 0, 50.0f);
    Player player(&shipTexture, initialPosX, initialPosY, 200.0f);
    std::vector<Bullet> bullets;
    std::vector<Obstacle> meteors;
    std::vector<Enemy> enemies;
    std::vector<Bullet> enemyBullets;
    std::vector<ComplexEnemy> complexEnemies;
    std::vector<Upgrade> upgrades;
    std::vector<LaserTurret> laserTurrets;
    std::vector<HeavyEnemy> heavyEnemies;

    int destroyedMeteors = 0;
    int maxMeteors = 10;
    int maxEnemies = 3;

    bool gameOver = false;
    bool newLevel = true;

    PlayerData playerData;
    Scoreboard scoreboard;
    std::string inputName = "";
    bool showCursor = true;
    float cursorBlinkTime = 0.0f;
    float gameTimeElapsed = 0.0f;
    int infiniteScore = 0;

    const int MAX_NAME_LENGTH = 15;
    float nameInputTimer = 0.0f;

    static int selectedOption = 0;
    static int selectedMode = 0;
    static int selectedDiff = 1;
    static int selectedPauseOption = 0;

    const char* options[] = { "PLAY GAME", "SCOREBOARD", "EXIT" };
    const int numOptions = 3;

    const char* modeOptions[] = { "CLASSIC MODE", "INFINITE MODE", "BACK" };
    const int numModeOptions = 3;

    const char* diffOptions[] = { "EASY", "MEDIUM", "HARD", "BACK" };
    const int numDiffOptions = 4;

    const char* pauseOptions[] = { "RESUME", "QUIT TO MENU" };
    const int numPauseOptions = 2;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        gameTimeElapsed += deltaTime;

        if (musicPlaying && !IsSoundPlaying(backgroundMusic)) {
        PlaySound(backgroundMusic);
        }

        switch (gameStateManager.getCurrentState()) {

        case GameState::MAIN_MENU:
        {
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("SPACE SHOOTER", WIDTH / 2 - MeasureText("SPACE SHOOTER", 60) / 2, HEIGHT / 4, 60, GREEN);

            for (int i = 0; i < numOptions; i++) {
                Color optionColor = (i == selectedOption) ? GREEN : GRAY;
                DrawText(options[i], WIDTH / 2 - MeasureText(options[i], 30) / 2, HEIGHT / 2 + i * 60, 30, optionColor);
            }

            if (IsKeyPressed(KEY_UP)) {
                selectedOption = (selectedOption > 0) ? selectedOption - 1 : numOptions - 1;
                PlaySound(blipSelectSound);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selectedOption = (selectedOption + 1) % numOptions;
                PlaySound(blipSelectSound);
            }

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(blipSelectSound);
                switch (selectedOption) {
                case 0:
                    gameStateManager.setCurrentState(GameState::MODE_SELECTION);
                    break;
                case 1:
                    gameStateManager.setCurrentState(GameState::SCOREBOARD);
                    break;
                case 2:
                    gameStateManager.setCurrentState(GameState::EXIT);
                    break;
                }
            }

            EndDrawing();
        }
        break;

        case GameState::MODE_SELECTION:
        {
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("SELECT GAME MODE", WIDTH / 2 - MeasureText("SELECT GAME MODE", 40) / 2, HEIGHT / 4, 40, GREEN);

            for (int i = 0; i < numModeOptions; i++) {
                Color optionColor = (i == selectedMode) ? GREEN : GRAY;
                DrawText(modeOptions[i], WIDTH / 2 - MeasureText(modeOptions[i], 30) / 2, HEIGHT / 2 + i * 60, 30, optionColor);
            }

            if (IsKeyPressed(KEY_UP)) {
                selectedMode = (selectedMode > 0) ? selectedMode - 1 : numModeOptions - 1;
                PlaySound(blipSelectSound);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selectedMode = (selectedMode + 1) % numModeOptions;
                PlaySound(blipSelectSound);
            }

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(blipSelectSound);
                switch (selectedMode) {
                case 0:
                    gameStateManager.setCurrentState(GameState::CLASSIC_LEVEL_1);
                    newLevel = true;
                    gameOver = false;
                    break;
                case 1:
                    gameStateManager.setCurrentState(GameState::DIFFICULTY_SELECTION);
                    inputName = "";
                    break;
                case 2:
                    gameStateManager.setCurrentState(GameState::MAIN_MENU);
                    break;
                }
            }

            EndDrawing();
        }
        break;

        case GameState::NAME_INPUT:
        {
            cursorBlinkTime += deltaTime;
            if (cursorBlinkTime >= 0.5f) {
                showCursor = !showCursor;
                cursorBlinkTime = 0.0f;
            }
            
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (inputName.length() < MAX_NAME_LENGTH)) {
                    inputName += (char)key;
                    PlaySound(blipSelectSound);
                }
                key = GetCharPressed();
            }
            
            if (IsKeyPressed(KEY_BACKSPACE) && !inputName.empty()) {
                inputName.pop_back();
                PlaySound(hitHurtSound);
            }
            
            BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("ENTER YOUR NAME:", WIDTH / 2 - MeasureText("ENTER YOUR NAME:", 40) / 2, HEIGHT / 3, 40, GREEN);
            
            DrawRectangle(WIDTH / 2 - 200, HEIGHT / 2 - 25, 400, 50, DARKGRAY);
            DrawRectangleLines(WIDTH / 2 - 200, HEIGHT / 2 - 25, 400, 50, GREEN);
            
            std::string displayName = inputName;
            if (showCursor) {
                displayName += "|";
            }
            DrawText(displayName.c_str(), WIDTH / 2 - MeasureText(displayName.c_str(), 30) / 2, HEIGHT / 2 - 15, 30, WHITE);
            
            DrawText("Press ENTER to continue", WIDTH / 2 - MeasureText("Press ENTER to continue", 20) / 2, HEIGHT * 2 / 3, 20, GRAY);
            
            EndDrawing();
            
            if (IsKeyPressed(KEY_ENTER) && !inputName.empty()) {
                playerData.setPlayerName(inputName);
                gameStateManager.setCurrentState(GameState::INFINITE_MODE);
                newLevel = true;
                gameOver = false;
            }
        }
        break;

        case GameState::DIFFICULTY_SELECTION:
        {
            BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("SELECT DIFFICULTY", WIDTH / 2 - MeasureText("SELECT DIFFICULTY", 40) / 2, HEIGHT / 4, 40, GREEN);
            
            const char* diffOptions[] = { "EASY", "MEDIUM", "HARD", "BACK" };
            const int numDiffOptions = 4;
            
            for (int i = 0; i < numDiffOptions; i++) {
                Color optionColor = (i == selectedDiff) ? GREEN : GRAY;
                DrawText(diffOptions[i], WIDTH / 2 - MeasureText(diffOptions[i], 30) / 2, HEIGHT / 2 + i * 60, 30, optionColor);
            }
            
            if (IsKeyPressed(KEY_UP)) {
                selectedDiff = (selectedDiff > 0) ? selectedDiff - 1 : numDiffOptions - 1;
                PlaySound(blipSelectSound);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selectedDiff = (selectedDiff + 1) % numDiffOptions;
                PlaySound(blipSelectSound);
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(blipSelectSound);
                switch (selectedDiff) {
                    case 0:
                        playerData.setDifficulty(Difficulty::EASY);
                        break;
                    case 1:
                        playerData.setDifficulty(Difficulty::MEDIUM);
                        break;
                    case 2:
                        playerData.setDifficulty(Difficulty::HARD);
                        break;
                    case 3:
                        gameStateManager.setCurrentState(GameState::MODE_SELECTION);
                        break;
                }
                playerData.resetScore();
                gameStateManager.setCurrentState(GameState::NAME_INPUT);
            }
            
            EndDrawing();
        }
        break;

        case GameState::CLASSIC_LEVEL_1:
        {
            if (!gameOver) {
                if (newLevel) {
                    ShowLevelTransitionScreen("LEVEL 1", WIDTH, HEIGHT);
                    newLevel = false;
                    player.posx = initialPosX;
                    player.posy = initialPosY;
                    destroyedMeteors = 0;
                    player.SetHealth(3);
                    meteors.clear();
                }

                for (size_t i = 0; i < meteors.size(); i++) {
                    if (!meteors[i].isActive) {
                        meteors.erase(meteors.begin() + i);
                        destroyedMeteors++;
                        i--;
                        if (destroyedMeteors >= maxMeteors) {
                            gameStateManager.setCurrentState(GameState::CLASSIC_LEVEL_2);
                            newLevel = true;
                        }
                    }
                }

                while (meteors.size() < maxMeteors) {
                    CreateMeteor(meteors, &smallMeteorTexture, &mediumMeteorTexture, &largeMeteorTexture, GetRandomValue(0, 2), 0, 0, player.posx, player.posy, 100.0f);
                }

                for (auto& meteor : meteors) {
                    if (meteor.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                        meteor.isActive = false;
                        player.SetHealth(player.GetHealth()-1);
                    }
                }

                for (auto& bullet : bullets) {
                    bool bulletHit = false;
                    
                    for (auto& meteor : meteors) {
                        if (bullet.isActive && meteor.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            meteor.isActive = false;
                            PlaySound(hitEnemySound);
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            bulletHit = true;
                        }
                    }
                    
                    for (auto& enemy : complexEnemies) {
                        if (bullet.isActive && enemy.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                            enemy.isActive = false;
                            PlaySound(hitEnemySound);
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            bulletHit = true;
                        }
                    }
                }

                player.Event(bullets, &bulletTexture, 300.0f, &laserShootSound);
                player.Update();
                bg.Update();
                for (auto& bullet : bullets) {
                    bullet.Update();
                    if (bullet.posy > GetScreenHeight()) {
                        bullet.isActive = false;
                    }
                }
                bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet& b) { return !b.isActive; }),
                    bullets.end());
                for (auto& meteor : meteors) meteor.Update();

                if (player.GetHealth() <= 0) gameOver = true;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            bg.Draw();
            for (auto& meteor : meteors) {
                meteor.Draw();
            }
            for (auto& bullet : bullets) {
                bullet.Draw();
            }
            player.Draw();
            for (int i = 0; i < player.GetHealth(); i++) {
                DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
            }
            EndDrawing();
            if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);
        }
        break;

        case GameState::CLASSIC_LEVEL_2:
        {
            if (!gameOver) {
                if (newLevel) {
                    ShowLevelTransitionScreen("LEVEL 2", WIDTH, HEIGHT);
                    newLevel = false;
                    player.SetHealth(3);
                    player.posx = initialPosX;
                    player.posy = initialPosY;
                    maxMeteors = 20;
                    meteors.clear();
                }
                if (canSpawnEnemies) {
                    enemies.clear();
                    for (int i = 0; i < 5; i++) { 
                        float posX = GetRandomValue(0, WIDTH - 32);
                        float posY = GetRandomValue(0, HEIGHT / 8); 

                        posX = std::min(static_cast<float>(WIDTH - 32), std::max(0.0f, posX));
                        posY = std::min(static_cast<float>(HEIGHT / 2), std::max(0.0f, posY));

                        enemies.emplace_back(&enemyTexture, posX, posY, GetRandomValue(100, 200));
                    }
                    canSpawnEnemies = false;
                }

                while (meteors.size() < maxMeteors) {
                    CreateMeteor(meteors, &smallMeteorTexture, &mediumMeteorTexture, &largeMeteorTexture, GetRandomValue(0, 2), 0, 0, player.posx, player.posy, 100.0f);
                }

                for (auto& meteor : meteors) meteor.Update();

                for (auto& enemy : enemies) {
                    enemy.Update();
                    if (enemy.isActive) {
                        if (enemy.CanShoot(GetFrameTime())) {
                            Bullet newBullet(&enemyBulletTexture, enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size, 4, { 0, 1 }, 200.0f);
                            enemyBullets.push_back(newBullet);
                        }
                    }
                }

                for (auto& bullet : enemyBullets) {
                    bullet.Update();
                    if (bullet.posy > GetScreenHeight()) {
                        bullet.isActive = false;
                    }
                }

                enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                    [](Bullet& b) { return !b.isActive; }),
                    enemyBullets.end());

                for (auto& bullet : enemyBullets) {
                    if (bullet.isActive && CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                        { player.posx, player.posy }, player.size / 2.0f)) {
                        bullet.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                    }
                }

                for (auto& meteor : meteors) {
                    if (meteor.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                        meteor.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                    }
                }
                
                for (auto& bullet : bullets) {
                    bool bulletHit = false;
                    
                    for (auto& meteor : meteors) {
                        if (bullet.isActive && meteor.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            meteor.isActive = false;
                            PlaySound(hitEnemySound);
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            bulletHit = true;
                        }
                    }
                    
                    for (auto& enemy : complexEnemies) {
                        if (bullet.isActive && enemy.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                            enemy.isActive = false;
                            PlaySound(hitEnemySound);
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            bulletHit = true;
                        }
                    }
                }

                meteors.erase(std::remove_if(meteors.begin(), meteors.end(), [](const Obstacle& m) {
                    return !m.isActive;
                    }), meteors.end());

                for (auto& bullet : bullets) {
                    for (auto& enemy : enemies) {
                        if (bullet.isActive && enemy.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f }, enemy.size / 2.0f)) {
                            bullet.isActive = false;
                            enemy.isActive = false;
                        }
                    }
                }

                enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) {
                    return !e.isActive; 
                    }), enemies.end());

                for (auto& enemy : enemies) {
                    if (enemy.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f }, enemy.size / 2.0f)) {
                        enemy.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                    }
                }

                if (player.GetHealth() <= 0) gameOver = true;

                if (enemies.empty()) {
                    gameStateManager.setCurrentState(GameState::CLASSIC_LEVEL_3);
                    newLevel = true;
                }

                player.Event(bullets, &bulletTexture, 300.0f, &laserShootSound);
                player.Update();
                bg.Update();
                for (auto& bullet : bullets) {
                    bullet.Update();
                    if (bullet.posy > GetScreenHeight()) {
                        bullet.isActive = false;
                    }
                }
                bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet& b) { return !b.isActive; }),
                    bullets.end());
                if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);

                BeginDrawing();
                ClearBackground(RAYWHITE);
                bg.Draw();
                for (auto& meteor : meteors) {
                    meteor.Draw();
                }
                for (auto& enemy : enemies) {
                    enemy.Draw();
                }
                for (auto& bullet : bullets) {
                    bullet.Draw();
                }
                for (auto& bullet : enemyBullets) {
                    bullet.Draw();
                }
                player.Draw();
                for (int i = 0; i < player.GetHealth(); i++) {
                    DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
                }

                EndDrawing();
                if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);
            }
        }
        break;

        case GameState::CLASSIC_LEVEL_3:
        {
            if (!gameOver) {
                if (newLevel) {
                    ShowLevelTransitionScreen("LEVEL 3", WIDTH, HEIGHT);
                    newLevel = false;
                    player.SetHealth(3);
                    player.posx = initialPosX;
                    player.posy = initialPosY;
                    maxMeteors = 20;
                    meteors.clear();
                }
                if (canSpawnEnemies) {
                    enemies.clear();
                    for (int i = 0; i < 8; i++) {
                        float posX = GetRandomValue(0, WIDTH - 32);
                        float posY = GetRandomValue(0, HEIGHT / 16);
                        float speed = static_cast<float>(GetRandomValue(100, 200));
                        complexEnemies.push_back(ComplexEnemy(&enemyTexture, posX, posY, speed));
                    }
                    canSpawnEnemies = false;
                }
                    while (meteors.size() < maxMeteors) {
                        CreateMeteor(meteors, &smallMeteorTexture, &mediumMeteorTexture, &largeMeteorTexture, GetRandomValue(0, 2), 0, 0, player.posx, player.posy, 200.0f);
                    }

                    for (auto& meteor : meteors) meteor.Update();

                    for (auto& enemy : complexEnemies) {
                        enemy.SetPlayerPosition({ player.posx, player.posy });
                        enemy.Update(complexEnemies);
                        if (enemy.isActive) {
                            if (enemy.CanShoot(GetFrameTime())) {
                                Vector2 bulletDirection = { cos((enemy.rotation + 90) * DEG2RAD), sin((enemy.rotation + 90) * DEG2RAD) };
                                Bullet newBullet(&enemyBulletTexture, enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f, 4, bulletDirection, 350.0f);
                                enemyBullets.push_back(newBullet);
                            }
                        }
                    }

                    complexEnemies.erase(std::remove_if(complexEnemies.begin(), complexEnemies.end(), [](const ComplexEnemy& e) {
                        return !e.isActive;
                        }), complexEnemies.end());

                    for (auto& bullet : enemyBullets) {
                        bullet.Update();
                        if (bullet.posy > GetScreenHeight()) {
                            bullet.isActive = false;
                        }
                    }

                    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                        [](Bullet& b) { return !b.isActive; }),
                        enemyBullets.end());

                    for (auto& bullet : enemyBullets) {
                        if (bullet.isActive && CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                            { player.posx, player.posy }, player.size / 2.0f)) {
                            bullet.isActive = false;
                            player.SetHealth(player.GetHealth() - 1);
                        }
                    }

                    for (auto& meteor : meteors) {
                        if (meteor.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                            { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            meteor.isActive = false;
                            player.SetHealth(player.GetHealth() - 1);
                        }
                    }

                    for (auto& bullet : bullets) {
                        bool bulletHit = false;
                        
                        for (auto& meteor : meteors) {
                            if (bullet.isActive && meteor.isActive &&
                                CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                    { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                                meteor.isActive = false;
                                PlaySound(hitEnemySound);
                                
                                if (!bullet.isPiercing) {
                                    bullet.isActive = false;
                                } else {
                                    bullet.piercingHits++;
                                    if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                        bullet.isActive = false;
                                    }
                                }
                                bulletHit = true;
                            }
                        }
                        
                        for (auto& enemy : complexEnemies) {
                            if (bullet.isActive && enemy.isActive &&
                                CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                    { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                                enemy.isActive = false;
                                PlaySound(hitEnemySound);
                                
                                if (!bullet.isPiercing) {
                                    bullet.isActive = false;
                                } else {
                                    bullet.piercingHits++;
                                    if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                        bullet.isActive = false;
                                    }
                                }
                                bulletHit = true;
                            }
                        }
                    }

                    if (player.GetHealth() <= 0) gameOver = true;


                    for (auto& enemy : complexEnemies) {
                        if (enemy.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                            { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                            enemy.isActive = false;
                            player.SetHealth(player.GetHealth() - 1);
                        }
                    }
                    if (complexEnemies.empty()) {
                        gameStateManager.setCurrentState(GameState::WIN);
                    }
                    player.Event(bullets, &bulletTexture, 300.0f, &laserShootSound);
                    player.Update();
                    bg.Update();
                    for (auto& bullet : bullets) {
                        bullet.Update();
                        if (bullet.posy > GetScreenHeight()) {
                            bullet.isActive = false;
                        }
                    }
                    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                        [](Bullet& b) { return !b.isActive; }),
                        bullets.end());

                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    bg.Draw();
                    for (auto& meteor : meteors) {
                        meteor.Draw();
                    }
                    for (auto& enemy : complexEnemies) {
                        enemy.Draw();
                    }
                    for (auto& bullet : bullets) {
                        bullet.Draw();
                    }
                    for (auto& bullet : enemyBullets) {
                        bullet.Draw();
                    }
                    player.Draw();
                    for (int i = 0; i < player.GetHealth(); i++) {
                        DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
                    }
                    EndDrawing();
                    if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);
                }
        }
        break;

        case GameState::INFINITE_MODE:
        {
            if (!gameOver) {
                if (newLevel) {
                    player.posx = initialPosX;
                    player.posy = initialPosY;
                    player.SetHealth(5);
                    meteors.clear();
                    enemies.clear();
                    complexEnemies.clear();
                    bullets.clear();
                    enemyBullets.clear();
                    laserTurrets.clear();
                    heavyEnemies.clear();  
                    
                    int numEnemies, numMeteors, numLaser, numHeavy;
                    float enemySpeed, meteorSpeed;
                    switch (playerData.getDifficulty()) {
                        case Difficulty::EASY:
                            numEnemies = 2;
                            maxEnemies = 2;
                            numMeteors = 8;
                            enemySpeed = 100.0f;
                            meteorSpeed = 150.0f;
                            numLaser = 1;
                            numHeavy = 0;
                            break;
                        case Difficulty::MEDIUM:
                            numEnemies = 3;
                            maxEnemies = 3;
                            numMeteors = 10;
                            enemySpeed = 150.0f;
                            meteorSpeed = 200.0f;
                            numLaser = 2;
                            numHeavy = 1;
                            break;
                        case Difficulty::HARD:
                            numEnemies = 5;
                            maxEnemies = 5;
                            numMeteors = 12;
                            enemySpeed = 200.0f;
                            meteorSpeed = 250.0f;
                            numLaser = 2;
                            numHeavy = 2;
                            break;
                    }
                    
                    for (int i = 0; i < numEnemies; i++) {
                        float posX = GetRandomValue(0, WIDTH - 32);
                        float posY = GetRandomValue(0, HEIGHT / 8);
                        complexEnemies.push_back(ComplexEnemy(&enemyTexture, posX, posY, enemySpeed));
                    }
                    
                    for (int i = 0; i < numHeavy; i++) {
                        float posX = GetRandomValue(WIDTH / 4, 3 * WIDTH / 4);
                        float posY = GetRandomValue(50, HEIGHT / 4);
                        heavyEnemies.emplace_back(&enemyTexture, posX, posY, enemySpeed);
                    }
                    if (numLaser >= 1) {
                        laserTurrets.emplace_back(&laserTexture, 10, HEIGHT / 3, true);
                    }
                    if (numLaser >= 2) {
                        laserTurrets.emplace_back(&laserTexture, WIDTH - 42, HEIGHT / 2, false);
                    }

                    maxMeteors = numMeteors;
                    while (meteors.size() < maxMeteors) {
                        CreateMeteor(meteors, &smallMeteorTexture, &mediumMeteorTexture, &largeMeteorTexture, 
                                     GetRandomValue(0, 2), 0, 0, player.posx, player.posy, 200.0f);
                    }
                    
                    newLevel = false;
                }
                
                int activeTurrets = 0;
                for (auto& turret : laserTurrets) {
                    if (turret.isActive) activeTurrets++;
                }
                
                int expectedTurrets = 0;
                switch (playerData.getDifficulty()) {
                    case Difficulty::EASY: expectedTurrets = 1; break;
                    case Difficulty::MEDIUM: expectedTurrets = 2; break;
                    case Difficulty::HARD: expectedTurrets = 2; break;
                }
                
                if (activeTurrets < expectedTurrets) {
                    static float respawnTimer = 0.0f;
                    respawnTimer += GetFrameTime();
                    
                    if (respawnTimer >= 10.0f) {
                        bool hasLeftTurret = false;
                        bool hasRightTurret = false;
                        
                        for (auto& turret : laserTurrets) {
                            if (turret.isActive) {
                                if (turret.isLeftSide) hasLeftTurret = true;
                                else hasRightTurret = true;
                            }
                        }
                        
                        if (!hasLeftTurret && expectedTurrets >= 1) {
                            laserTurrets.emplace_back(&laserTexture, 10, HEIGHT / 3, true);
                        }
                        
                        if (!hasRightTurret && expectedTurrets >= 2) {
                            laserTurrets.emplace_back(&laserTexture, WIDTH - 58, HEIGHT / 2, false);
                        }
                        
                        respawnTimer = 0.0f;
                    }
                }
                
                for (auto& meteor : meteors) {
                        float originalSpeed = meteor.speed;
                        if (player.HasSlowEnemies()) {
                            meteor.speed *= 0.2f;
                        }
                        
                        meteor.Update();
                        
                        meteor.speed = originalSpeed;
                }
                for (auto& turret : laserTurrets) {
                    turret.Update();
                    if (turret.CanShoot(GetFrameTime())) {
                        turret.StartLaser();
                        PlaySound(laserBeamSound);
                    }
                }

                while (meteors.size() < maxMeteors) {
                    CreateMeteor(meteors, &smallMeteorTexture, &mediumMeteorTexture, &largeMeteorTexture, 
                                GetRandomValue(0, 2), 0, 0, player.posx, player.posy, 200.0f);
                }
                std::vector<ComplexEnemy*> allEnemiesForUpdate;
                for (auto& enemy : complexEnemies) {
                    allEnemiesForUpdate.push_back(&enemy);
                }
                for (auto& enemy : heavyEnemies) {
                    allEnemiesForUpdate.push_back(&enemy);
                }
                
                for (auto& enemy : heavyEnemies) {
                    if (!enemy.isActive) continue;
                    
                    enemy.SetPlayerPosition({ player.posx, player.posy });
                    
                    float originalSpeed = enemy.speed;
                    if (player.HasSlowEnemies()) {
                        enemy.speed *= 0.5f;
                    }
                    
                    std::vector<ComplexEnemy> tempEnemies;
                    for (auto& ce : complexEnemies) {
                        if (ce.isActive) tempEnemies.push_back(ce);
                    }
                    
                    enemy.Update(tempEnemies);
                    enemy.speed = originalSpeed;
                    
                    if (enemy.isActive && enemy.CanShoot(GetFrameTime())) {
                        Vector2 bulletDirection = { cos((enemy.rotation + 90) * DEG2RAD), sin((enemy.rotation + 90) * DEG2RAD) };
                        Bullet newBullet(&enemyBulletTexture, enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f, 8, bulletDirection, 300.0f);
                        enemyBullets.push_back(newBullet);
                        PlaySound(heavyHitSound);
                    }
                }
                for (auto& enemy : complexEnemies) {
                enemy.SetPlayerPosition({ player.posx, player.posy });
                
                float originalSpeed = enemy.speed;
                if (player.HasSlowEnemies()) {
                    enemy.speed *= 0.5f;
                }
                
                enemy.Update(complexEnemies);
                
                enemy.speed = originalSpeed;
                
                if (enemy.isActive) {
                    if (enemy.CanShoot(GetFrameTime())) {
                        Vector2 bulletDirection = { cos((enemy.rotation + 90) * DEG2RAD), sin((enemy.rotation + 90) * DEG2RAD) };
                        Bullet newBullet(&enemyBulletTexture, enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f, 4, bulletDirection, 350.0f);
                        enemyBullets.push_back(newBullet);
                        PlaySound(enemyShootSound);
                    }
                }
            }

                for (auto& bullet : enemyBullets) {
                    bullet.Update();
                    if (bullet.posy > GetScreenHeight()) {
                        bullet.isActive = false;
                    }
                }

                enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                    [](Bullet& b) { return !b.isActive; }),
                    enemyBullets.end());

                for (auto& turret : laserTurrets) {
                    if (turret.isActive && turret.isShooting && !player.IsShielded()) {
                        Rectangle laserRect = turret.GetLaserRect();
                        Rectangle playerRect = {
                            player.posx - player.size/2,
                            player.posy - player.size/2,
                            player.size,
                            player.size
                        };
                        
                        if (CheckCollisionRecs(laserRect, playerRect)) {
                            player.SetHealth(player.GetHealth() - 1);
                            PlaySound(hitHurtSound);
                            turret.StopLaser();
                        }
                    }
                }
                for (auto& bullet : bullets) {
                    if (!bullet.isActive) continue;
                    
                    for (auto& enemy : heavyEnemies) {
                        if (!enemy.isActive) continue;
                        
                        if (CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                        { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                            
                            bool enemyDestroyed = enemy.TakeDamage();
                            PlaySound(hitEnemySound);
                            
                            if (enemyDestroyed) {
                                playerData.addScore(100);
                            }
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            break;
                        }
                    }
                }

                for (auto& enemy : heavyEnemies) {
                    if (enemy.isActive && !player.IsShielded() && 
                        CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                        enemy.isActive = false;
                        player.SetHealth(player.GetHealth() - 2);
                        PlaySound(hitHurtSound);
                    }
                }

                for (auto& bullet : bullets) {
                    for (auto& turret : laserTurrets) {
                        if (bullet.isActive && turret.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                            { turret.posx + turret.size / 2.0f, turret.posy + turret.size / 2.0f }, turret.size / 2.0f)) {
                            turret.isActive = false;
                            bullet.isActive = false;
                            playerData.addScore(75);
                            PlaySound(hitEnemySound);
                        }
                    }
                }
                for (auto& bullet : bullets) {
                    bool bulletHit = false;

                    for (auto& meteor : meteors) {
                        if (bullet.isActive && meteor.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            meteor.isActive = false;
                            PlaySound(hitEnemySound); 
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            bulletHit = true;
                        }
                    }
                    
                    for (auto& enemy : complexEnemies) {
                        if (bullet.isActive && enemy.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                            enemy.isActive = false;
                            PlaySound(hitEnemySound);
                            
                            if (!bullet.isPiercing) {
                                bullet.isActive = false;
                            } else {
                                bullet.piercingHits++;
                                if (bullet.piercingHits >= bullet.maxPiercingHits) {
                                    bullet.isActive = false;
                                }
                            }
                            bulletHit = true;
                        }
                    }
                }

                for (auto& bullet : enemyBullets) {
                    if (bullet.isActive && !player.IsShielded() && 
                        CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                        { player.posx, player.posy }, player.size / 2.0f)) {
                        bullet.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                        PlaySound(hitHurtSound);
                    }
                }

                for (auto& meteor : meteors) {
                    if (meteor.isActive && !player.IsShielded() && 
                        CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                        meteor.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                        PlaySound(hitHurtSound);
                    }
                }

                for (auto& enemy : complexEnemies) {
                    if (enemy.isActive && !player.IsShielded() && 
                        CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                        enemy.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                        PlaySound(hitHurtSound);
                    }
                }

                for (auto& upgrade : upgrades) {
                    upgrade.Update();
                }

                upgrades.erase(std::remove_if(upgrades.begin(), upgrades.end(),
                    [](const Upgrade& u) { return !u.isActive; }),
                    upgrades.end());

                for (auto& upgrade : upgrades) {
                    if (upgrade.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { upgrade.posx + upgrade.size / 2.0f, upgrade.posy + upgrade.size / 2.0f }, upgrade.size / 2.0f)) {
                        upgrade.isActive = false;
                        player.ApplyUpgrade((int)upgrade.type);
                        PlaySound(powerUpSound);
                    }
                }
                
                for (size_t i = 0; i < meteors.size(); i++) {
                    if (!meteors[i].isActive) {
                        CreateUpgradeDrop(upgrades, &upgradeTexture, meteors[i].posx, meteors[i].posy);
                        meteors.erase(meteors.begin() + i);
                        playerData.addScore(10);
                        i--;
                    }
                }

                for (size_t i = 0; i < complexEnemies.size(); i++) {
                    if (!complexEnemies[i].isActive) {
                        CreateUpgradeDrop(upgrades, &upgradeTexture, complexEnemies[i].posx, complexEnemies[i].posy);
                        complexEnemies.erase(complexEnemies.begin() + i);
                        playerData.addScore(50);
                        i--;
                    }
                }
                
                if (player.GetHealth() <= 0) {
                    gameOver = true;
                }

                player.Event(bullets, &bulletTexture, 300.0f, &laserShootSound);
                player.Update();
                bg.Update();

                if (player.GetHealth() <= 0) {
                    gameOver = true;
                }

                for (auto& bullet : bullets) {
                    bullet.Update();
                    if (bullet.posy > GetScreenHeight()) {
                        bullet.isActive = false;
                    }
                }

                bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet& b) { return !b.isActive; }),
                    bullets.end());

                for (size_t i = 0; i < meteors.size(); i++) {
                    if (!meteors[i].isActive) {
                        meteors.erase(meteors.begin() + i);
                        playerData.addScore(10);
                        i--;
                    }
                }

                for (size_t i = 0; i < complexEnemies.size(); i++) {
                    if (!complexEnemies[i].isActive) {
                        complexEnemies.erase(complexEnemies.begin() + i);
                        playerData.addScore(50);
                        i--;
                    }
                }

                if (complexEnemies.size() < maxEnemies) {
                    float posX = GetRandomValue(0, WIDTH - 32);
                    float posY = GetRandomValue(0, HEIGHT / 8);
                    float speed;
                    switch (playerData.getDifficulty()) {
                        case Difficulty::EASY: speed = 100.0f; break;
                        case Difficulty::MEDIUM: speed = 150.0f; break;
                        case Difficulty::HARD: speed = 200.0f; break;
                    }
                    complexEnemies.push_back(ComplexEnemy(&enemyTexture, posX, posY, speed));
                }

                if (IsKeyPressed(KEY_P)) {
                    gameStateManager.setCurrentState(GameState::PAUSE);
                }
            }
            
            BeginDrawing();
            ClearBackground(RAYWHITE);
            bg.Draw();
            
            for (auto& meteor : meteors) meteor.Draw();
            for (auto& enemy : complexEnemies) enemy.Draw();
            for (auto& bullet : bullets) bullet.Draw();
            for (auto& bullet : enemyBullets) bullet.Draw();
            for (auto& upgrade : upgrades) upgrade.Draw();
            for (auto& turret : laserTurrets) turret.Draw();
            for (auto& heavy : heavyEnemies) heavy.Draw();
            player.Draw();
            
            std::string scoreText = "SCORE: " + std::to_string(playerData.getScore());
            DrawText(scoreText.c_str(), 10, 50, 20, WHITE);
            
            DrawText(playerData.getPlayerName().c_str(), 10, 80, 20, WHITE);
            

            int yOffset = 110;
            if (player.GetCurrentShootCooldown() < 0.5f) {
                DrawText("FAST SHOOTING ACTIVE!", 10, yOffset, 16, RED);
                yOffset += 20;
            }
            if (player.GetCurrentBulletSize() > 1.0f) {
                DrawText("BIG BULLETS ACTIVE!", 10, yOffset, 16, BLUE);
                yOffset += 20;
            }
            if (player.HasTripleShot()) {
                DrawText("TRIPLE SHOT ACTIVE!", 10, yOffset, 16, YELLOW);
                yOffset += 20;
            }
            if (player.HasPiercingBullets()) {
                DrawText("PIERCING BULLETS ACTIVE!", 10, yOffset, 16, ORANGE);
                yOffset += 20;
            }
            if (player.IsShielded()) {
                DrawText("SHIELD ACTIVE!", 10, yOffset, 16, SKYBLUE);
                yOffset += 20;
            }
            if (player.GetCurrentSpeed() > 200.0f) {
                DrawText("FAST MOVEMENT ACTIVE!", 10, yOffset, 16, GREEN);
                yOffset += 20;
            }
            if (player.HasSlowEnemies()) {
                DrawText("SLOW ENEMIES ACTIVE!", 10, yOffset, 16, PURPLE);
                yOffset += 20;
            }
            
            for (int i = 0; i < player.GetHealth(); i++) {
                DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
            }
            
            EndDrawing();
            
            if (gameOver) {
                gameStateManager.setCurrentState(GameState::GAME_OVER);
            }
        }
        break;

        case GameState::SCOREBOARD:
        {
            BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("HIGH SCORES", WIDTH / 2 - MeasureText("HIGH SCORES", 40) / 2, 60, 40, GREEN);
            
            const std::vector<PlayerScore>& scores = scoreboard.getScores();
            
            if (scores.empty()) {
                DrawText("No scores yet!", WIDTH / 2 - MeasureText("No scores yet!", 30) / 2, HEIGHT / 2, 30, WHITE);
            } else {
                DrawText("RANK", 100, 150, 20, GRAY);
                DrawText("NAME", 200, 150, 20, GRAY);
                DrawText("SCORE", 500, 150, 20, GRAY);
                DrawText("DIFFICULTY", 650, 150, 20, GRAY);
                
                for (size_t i = 0; i < scores.size(); i++) {
                    std::string rank = std::to_string(i + 1) + ".";
                    DrawText(rank.c_str(), 100, 200 + (int)i * 40, 20, WHITE);
                    
                    DrawText(scores[i].name.c_str(), 200, 200 + (int)i * 40, 20, WHITE);
                    
                    std::string score = std::to_string(scores[i].score);
                    DrawText(score.c_str(), 500, 200 + (int)i * 40, 20, WHITE);
                    
                    std::string diff;
                    switch (scores[i].difficulty) {
                        case Difficulty::EASY:
                            diff = "EASY";
                            break;
                        case Difficulty::MEDIUM:
                            diff = "MEDIUM";
                            break;
                        case Difficulty::HARD:
                            diff = "HARD";
                            break;
                    }
                    DrawText(diff.c_str(), 650, 200 + (int)i * 40, 20, WHITE);
                }
            }
            
            DrawText("Press BACKSPACE to return", WIDTH / 2 - MeasureText("Press BACKSPACE to return", 20) / 2, HEIGHT - 50, 20, GRAY);
            
            EndDrawing();
            
            if (IsKeyPressed(KEY_BACKSPACE)) {
                gameStateManager.setCurrentState(GameState::MAIN_MENU);
            }
        }
        break;

        case GameState::PAUSE:
        {
            BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("PAUSED", WIDTH / 2 - MeasureText("PAUSED", 40) / 2, HEIGHT / 3, 40, WHITE);
            
            for (int i = 0; i < numPauseOptions; i++) {
                Color optionColor = (i == selectedPauseOption) ? GREEN : GRAY;
                DrawText(pauseOptions[i], WIDTH / 2 - MeasureText(pauseOptions[i], 30) / 2, HEIGHT / 2 + i * 60, 30, optionColor);
            }
            
            if (IsKeyPressed(KEY_UP)) {
                selectedPauseOption = (selectedPauseOption > 0) ? selectedPauseOption - 1 : numPauseOptions - 1;
                PlaySound(blipSelectSound);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selectedPauseOption = (selectedPauseOption + 1) % numPauseOptions;
                PlaySound(blipSelectSound);
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(blipSelectSound);
                switch (selectedPauseOption) {
                    case 0:
                        if (gameStateManager.getCurrentState() == GameState::PAUSE) {
                            gameStateManager.setCurrentState(GameState::INFINITE_MODE);
                        }
                        break;
                    case 1:
                        if (gameStateManager.getCurrentState() == GameState::PAUSE) {
                                if (gameStateManager.hasJustTransitioned() && gameStateManager.getPreviousState() == GameState::INFINITE_MODE) {
                                    scoreboard.addScore(PlayerScore(playerData.getPlayerName(), playerData.getScore(), playerData.getDifficulty()));
                                    gameStateManager.markTransitionHandled();
                                    std::cout << "Score added: " << playerData.getPlayerName() << " - " << playerData.getScore() << std::endl;
                                }
                            ResetGameState(player, bullets, enemies, complexEnemies, enemyBullets, meteors, upgrades, laserTurrets, heavyEnemies, playerData, initialPosX, initialPosY);
                        }
                        gameStateManager.setCurrentState(GameState::MAIN_MENU);
                        break;
                }
            }
            
            if (IsKeyPressed(KEY_P)) {
                gameStateManager.setCurrentState(GameState::INFINITE_MODE);
            }
            
            EndDrawing();
        }
        break;

        case GameState::GAME_OVER:
        {
            if (gameStateManager.getPreviousState() == GameState::INFINITE_MODE) {
                if (gameStateManager.hasJustTransitioned() && gameStateManager.getPreviousState() == GameState::INFINITE_MODE) {
                                    scoreboard.addScore(PlayerScore(playerData.getPlayerName(), playerData.getScore(), playerData.getDifficulty()));
                                    gameStateManager.markTransitionHandled();
                                    std::cout << "Score added: " << playerData.getPlayerName() << " - " << playerData.getScore() << std::endl;
                                }
            }
            
            BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("Game Over", WIDTH / 2 - MeasureText("Game Over", 40) / 2, HEIGHT / 2 - 20, 40, RED);
            
            const char* restartMsg = "Press BACKSPACE to Restart";
            DrawText(restartMsg, WIDTH / 2 - MeasureText(restartMsg, 20) / 2, HEIGHT / 2 + 40, 20, WHITE);
            
            EndDrawing();
            ResetGameState(player, bullets, enemies, complexEnemies, enemyBullets, meteors, upgrades, laserTurrets, heavyEnemies, playerData, initialPosX, initialPosY);

            if (IsKeyPressed(KEY_BACKSPACE)) {
                gameOver = false;
                player.SetHealth(5);
                newLevel = true;
                gameStateManager.setCurrentState(GameState::MAIN_MENU);
            }
        }
        break;

        case GameState::WIN:
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("WIN", WIDTH / 2 - MeasureText("WIN", 40) / 2, HEIGHT / 2 - 20, 40, GREEN);
            DrawText("Press R to play again", WIDTH / 2 - MeasureText("Press R to play again", 20) / 2, HEIGHT / 2 + 40, 20, WHITE);
            EndDrawing();
            ResetGameState(player, bullets, enemies, complexEnemies, enemyBullets, meteors, upgrades, laserTurrets, heavyEnemies, playerData, initialPosX, initialPosY);

            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                player.SetHealth(3);
                newLevel = true;
                gameStateManager.setCurrentState(GameState::MAIN_MENU);
            }
        }
        break;

        case GameState::EXIT:
        {
            CloseWindow();
            return 0;
        }
        break;
        }

    }
    UnloadTexture(backgroundTex);
    UnloadTexture(shipTexture);
    UnloadTexture(bulletTexture);
    UnloadTexture(smallMeteorTexture);
    UnloadTexture(mediumMeteorTexture);
    UnloadTexture(largeMeteorTexture);
    UnloadTexture(enemyTexture);
    UnloadTexture(enemyBulletTexture);
    UnloadTexture(healthTexture);
    UnloadTexture(upgradeTexture);
    UnloadTexture(laserTexture);
    
    UnloadSound(backgroundMusic);
    UnloadSound(blipSelectSound);
    UnloadSound(hitEnemySound);
    UnloadSound(hitHurtSound);
    UnloadSound(laserShootSound);
    UnloadSound(powerUpSound);
    UnloadSound(enemyShootSound);
    UnloadSound(laserBeamSound);
    UnloadSound(heavyHitSound);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}