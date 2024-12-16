#include "raylib.h"
#include "Background.h"
#include "Player.h"
#include "Bullet.h"
#include "Obstacle.h"
#include "GameStateManager.h"
#include "ComplexEnemy.h"
#include <vector>
#include <iostream>

bool canSpawnEnemies = false;

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

int main() {
    const int WIDTH = 1080;
    const int HEIGHT = 960;
    const float initialPosX = WIDTH / 2;
    const float initialPosY = HEIGHT - 64;

    InitWindow(WIDTH, HEIGHT, "Just Another Space Invaders");
    SetTargetFPS(60);

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

    Background bg(&backgroundTex, 0, 0, 50.0f);
    Player player(&shipTexture, initialPosX, initialPosY, 200.0f);
    std::vector<Bullet> bullets;
    std::vector<Obstacle> meteors;
    std::vector<Enemy> enemies;
    std::vector<Bullet> enemyBullets;
    std::vector<ComplexEnemy> complexEnemies;

    int destroyedMeteors = 0;
    int maxMeteors = 10;

    bool gameOver = false;
    bool newLevel = true;

    while (!WindowShouldClose()) {
        // for debugging
        //if (IsKeyDown(KEY_KP_1)) gameStateManager.setCurrentState(GameState::LEVEL_1);
        //if (IsKeyDown(KEY_KP_2)) gameStateManager.setCurrentState(GameState::LEVEL_2);
        //if (IsKeyDown(KEY_KP_3)) gameStateManager.setCurrentState(GameState::LEVEL_3);
        switch (gameStateManager.getCurrentState()) {

        case GameState::MAIN_MENU:
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Press ENTER to Start", WIDTH / 2 - MeasureText("Press ENTER to Start", 40) / 2, HEIGHT / 2 - 20, 40, GREEN);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                gameStateManager.setCurrentState(GameState::LEVEL_1);
                newLevel = true;
            }
            break;

        case GameState::LEVEL_1:
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

                // check destroyed meteors
                for (size_t i = 0; i < meteors.size(); i++) {
                    if (!meteors[i].isActive) {
                        meteors.erase(meteors.begin() + i);
                        destroyedMeteors++;
                        i--;
                        if (destroyedMeteors >= maxMeteors) {
                            gameStateManager.setCurrentState(GameState::LEVEL_2);
                            newLevel = true;
                        }
                    }
                }

                // spawn meteors to keep screen busy
                while (meteors.size() < maxMeteors) {
                    CreateMeteor(meteors, &smallMeteorTexture, &mediumMeteorTexture, &largeMeteorTexture, GetRandomValue(0, 2), 0, 0, player.posx, player.posy, 100.0f);
                }

                // player meteor coll
                for (auto& meteor : meteors) {
                    if (meteor.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                        meteor.isActive = false;
                        player.SetHealth(player.GetHealth()-1);
                    }
                }

                // bullet meteor coll
                for (auto& bullet : bullets) {
                    for (auto& meteor : meteors) {
                        if (bullet.isActive && meteor.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            bullet.isActive = false;
                            meteor.isActive = false;
                        }
                    }
                }


                player.Event(bullets, &bulletTexture, 300.0f);
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
                //DrawCircleLines(meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f, meteor.size / 2.0f, RED);
            }
            for (auto& bullet : bullets) {
                bullet.Draw();
                //DrawCircleLines(bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f, bullet.size / 2.0f, GREEN);
            }
            player.Draw();
            //DrawCircleLines(player.posx, player.posy, player.size / 2.0f, BLUE);
            for (int i = 0; i < player.GetHealth(); i++) {
                DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
                //std::cout << 10 + i * (healthTexture.width + 5) << std::endl;
            }
            EndDrawing();
            if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);
            break;

        case GameState::LEVEL_2:
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

                // enemy bullet player coll
                for (auto& bullet : enemyBullets) {
                    if (bullet.isActive && CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                        { player.posx, player.posy }, player.size / 2.0f)) {
                        bullet.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                    }
                }

                // meteor player coll
                for (auto& meteor : meteors) {
                    if (meteor.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                        meteor.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                    }
                }
                
                // bullet meteor coll
                for (auto& bullet : bullets) {
                    for (auto& meteor : meteors) {
                        if (bullet.isActive && meteor.isActive &&
                            CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            bullet.isActive = false;
                            meteor.isActive = false;
                        }
                    }
                }

                meteors.erase(std::remove_if(meteors.begin(), meteors.end(), [](const Obstacle& m) {
                    return !m.isActive;
                    }), meteors.end());

                // bullet enemy coll
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

                // enemy player coll
                for (auto& enemy : enemies) {
                    if (enemy.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                        { enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f }, enemy.size / 2.0f)) {
                        enemy.isActive = false;
                        player.SetHealth(player.GetHealth() - 1);
                    }
                }

                if (player.GetHealth() <= 0) gameOver = true;

                if (enemies.empty()) {
                    gameStateManager.setCurrentState(GameState::LEVEL_3);
                    newLevel = true;
                }

                player.Event(bullets, &bulletTexture, 300.0f);
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
                    //DrawCircleLines(meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f, meteor.size / 2.0f, RED);
                }
                for (auto& enemy : enemies) {
                    enemy.Draw();
                    //DrawCircleLines(enemy.posx + enemy.size / 2.0f, enemy.posy + enemy.size / 2.0f, enemy.size / 2.0f, YELLOW); // Enemy hitbox
                    // std::cout << "Spawning enemy at: (" << enemy.posx << ", " << enemy.posy << "), isActive: " << enemy.isActive << std::endl;
                }
                for (auto& bullet : bullets) {
                    bullet.Draw();
                    //DrawCircleLines(bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f, bullet.size / 2.0f, GREEN);
                }
                for (auto& bullet : enemyBullets) {
                    bullet.Draw();
                    //DrawCircleLines(bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f, bullet.size / 2.0f, YELLOW);
                }
                player.Draw();
                //DrawCircleLines(player.posx, player.posy, player.size / 2.0f, BLUE);
                for (int i = 0; i < player.GetHealth(); i++) {
                    DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
                }

                EndDrawing();
                if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);
            }
            break;

        case GameState::LEVEL_3:
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

                    // enemy player bullet coll
                    for (auto& bullet : enemyBullets) {
                        if (bullet.isActive && CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                            { player.posx, player.posy }, player.size / 2.0f)) {
                            bullet.isActive = false;
                            player.SetHealth(player.GetHealth() - 1);
                        }
                    }

                    // meteor player coll
                    for (auto& meteor : meteors) {
                        if (meteor.isActive && CheckCollision({ player.posx, player.posy }, player.size / 2.0f,
                            { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                            meteor.isActive = false;
                            player.SetHealth(player.GetHealth() - 1);
                        }
                    }

                    // meteor bullet coll
                    for (auto& bullet : bullets) {
                        for (auto& meteor : meteors) {
                            if (bullet.isActive && meteor.isActive &&
                                CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                    { meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f }, meteor.size / 2.0f)) {
                                bullet.isActive = false;
                                meteor.isActive = false;
                            }
                        }
                    }

                    // bullet enemy coll
                    for (auto& bullet : bullets) {
                        for (auto& enemy : complexEnemies) {
                            if (bullet.isActive && enemy.isActive &&
                                CheckCollision({ bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f }, bullet.size / 2.0f,
                                    { enemy.posx, enemy.posy }, enemy.size / 2.0f)) {
                                bullet.isActive = false;
                                enemy.isActive = false;
                            }
                        }
                    }

                    if (player.GetHealth() <= 0) gameOver = true;


                    // player enemy coll
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
                    player.Event(bullets, &bulletTexture, 300.0f);
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
                        //DrawCircleLines(meteor.posx + meteor.size / 2.0f, meteor.posy + meteor.size / 2.0f, meteor.size / 2.0f, RED);
                    }
                    for (auto& enemy : complexEnemies) {
                        enemy.Draw();
                        //DrawCircleLines(enemy.posx, enemy.posy, enemy.size / 2.0f, YELLOW); // Enemy hitbox
                    }
                    for (auto& bullet : bullets) {
                        bullet.Draw();
                        //DrawCircleLines(bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f, bullet.size / 2.0f, GREEN);
                    }
                    for (auto& bullet : enemyBullets) {
                        bullet.Draw();
                        //DrawCircleLines(bullet.posx + bullet.size / 2.0f, bullet.posy + bullet.size / 2.0f, bullet.size / 2.0f, YELLOW);
                    }
                    player.Draw();
                    //DrawCircleLines(player.posx, player.posy, player.size / 2.0f, BLUE);
                    for (int i = 0; i < player.GetHealth(); i++) {
                        DrawTexture(healthTexture, 10 + i * (healthTexture.width + 5), 10, WHITE);
                    }
                    EndDrawing();
                    if (gameOver) gameStateManager.setCurrentState(GameState::GAME_OVER);
                }
            break;

        case GameState::GAME_OVER:
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Game Over", WIDTH / 2 - MeasureText("Game Over", 40) / 2, HEIGHT / 2 - 20, 40, RED);
            DrawText("Press R to Restart", WIDTH / 2 - MeasureText("Press R to Restart", 20) / 2, HEIGHT / 2 + 40, 20, WHITE);
            EndDrawing();
            bullets.clear();
            enemies.clear();
            complexEnemies.clear();
            enemyBullets.clear();
            meteors.clear();

            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                player.SetHealth(3);
                newLevel = true;
                gameStateManager.setCurrentState(GameState::MAIN_MENU);
            }
            break;

        case GameState::WIN:
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("WIN", WIDTH / 2 - MeasureText("WIN", 40) / 2, HEIGHT / 2 - 20, 40, GREEN);
            DrawText("Press R to play again", WIDTH / 2 - MeasureText("Press R to play again", 20) / 2, HEIGHT / 2 + 40, 20, WHITE);
            EndDrawing();
            bullets.clear();
            enemies.clear();
            complexEnemies.clear();
            enemyBullets.clear();
            meteors.clear();

            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                player.SetHealth(3);
                newLevel = true;
                gameStateManager.setCurrentState(GameState::MAIN_MENU);
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
    CloseWindow();
    return 0;
}