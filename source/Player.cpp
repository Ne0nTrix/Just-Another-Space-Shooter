#include "Player.h"
#include "Bullet.h"
#include <cmath>
#include <vector>

Player::Player(Texture* texture, float posx, float posy, float speed)
: Entity(texture, posx, posy, 32), speed(speed), shootCooldown(0.5f), currentCooldown(0.0f), rotation(0.0f) {
    health = maxHealth;
}

void Player::Event(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed, Sound* shootSound)
{
    float currentSpeed = GetCurrentSpeed();
    
    if (IsKeyDown(KEY_W)) posy -= currentSpeed * GetFrameTime();
    if (IsKeyDown(KEY_S)) posy += currentSpeed * GetFrameTime();
    if (IsKeyDown(KEY_A)) posx -= currentSpeed * GetFrameTime();
    if (IsKeyDown(KEY_D)) posx += currentSpeed * GetFrameTime();
    
    if (currentCooldown > 0.0f) {
        currentCooldown -= GetFrameTime();
    }

    if (currentCooldown <= 0.0f && IsKeyPressed(KEY_SPACE)) {
        if (HasTripleShot()) {
            ShootTriple(bullets, bulletTexture, bulletSpeed);
        } else {
            Shoot(bullets, bulletTexture, bulletSpeed);
        }
        currentCooldown = GetCurrentShootCooldown();
        if (shootSound != nullptr) {
            PlaySound(*shootSound);
        }
    }
    RotateTowards(GetMousePosition());
}

void Player::Draw() const
{
    Vector2 center = { static_cast<float>(texture->width / 2.0), static_cast<float>(texture->height / 2.0) };
    Color playerTint = WHITE;
    if (IsShielded()) {
        float shieldAlpha = 0.3f + 0.2f * sin(GetTime() * 10.0f);
        DrawCircle((int)posx, (int)posy, size/2 + 8, Color{100, 200, 255, (unsigned char)(255 * shieldAlpha)});
        DrawCircle((int)posx, (int)posy, size/2 + 12, Color{150, 220, 255, (unsigned char)(100 * shieldAlpha)});
    }
    
    if (fasterShootingTimer > 0) {
        DrawCircle((int)posx, (int)posy, size/2 + 5, Color{255, 100, 100, 50});
    }
    if (biggerBulletsTimer > 0) {
        DrawCircle((int)posx, (int)posy, size/2 + 5, Color{100, 100, 255, 50});
    }
    if (tripleShotTimer > 0) {
        DrawCircle((int)posx, (int)posy, size/2 + 5, Color{255, 255, 100, 50});
    }
    if (fasterMovementTimer > 0) {
        DrawCircle((int)posx, (int)posy, size/2 + 5, Color{100, 255, 255, 50});
    }
    
    DrawTexturePro(*texture, { 0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height) },
        { posx, posy, static_cast<float>(texture->width), static_cast<float>(texture->height)}, center, rotation, playerTint);
}

void Player::Update()
{
    if (posx < texture->width / 2) posx = texture->width / 2;
    if (posx > GetScreenWidth() - texture->width/2) posx = GetScreenWidth() - texture->width/2;
    if (posy < texture->height / 2) posy = texture->height / 2;
    if (posy > GetScreenHeight() - texture->height/2) posy = GetScreenHeight() - texture->height/2;
    
    UpdateUpgrades();
}

void Player::RotateTowards(Vector2 target)
{
    float deltaX = target.x - posx;
    float deltaY = target.y - posy;
    rotation = atan2(deltaY, deltaX) * (180 / PI);
    rotation += 90;
}

void Player::Shoot(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed)
{
    float radians = (rotation - 90) * (PI / 180);
    Vector2 direction = { cos(radians), sin(radians) };
    
    float bulletSize = 4 * GetCurrentBulletSize();
    bool piercing = HasPiercingBullets();
    bullets.emplace_back(bulletTexture, posx, posy, bulletSize, direction, bulletSpeed, piercing);
}

void Player::ShootTriple(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed)
{
    float radians = (rotation - 90) * (PI / 180);
    float bulletSize = 4 * GetCurrentBulletSize();
    bool piercing = HasPiercingBullets();
    Vector2 direction1 = { cos(radians), sin(radians) };
    bullets.emplace_back(bulletTexture, posx, posy, bulletSize, direction1, bulletSpeed, piercing);
    float leftAngle = radians - (15 * PI / 180);
    Vector2 direction2 = { cos(leftAngle), sin(leftAngle) };
    bullets.emplace_back(bulletTexture, posx - 10, posy, bulletSize, direction2, bulletSpeed, piercing);
    float rightAngle = radians + (15 * PI / 180);
    Vector2 direction3 = { cos(rightAngle), sin(rightAngle) };
    bullets.emplace_back(bulletTexture, posx + 10, posy, bulletSize, direction3, bulletSpeed, piercing);
}

void Player::ApplyUpgrade(int upgradeType) {
    switch (upgradeType) {
        case 0:
            shootSpeedMultiplier = 0.3f;
            fasterShootingTimer = UPGRADE_DURATION;
            break;
        case 1:
            bulletSizeMultiplier = 2.0f;
            biggerBulletsTimer = UPGRADE_DURATION;
            break;
        case 2:
            if (health < maxHealth) {
                health++;
            }
            break;
        case 3:
            tripleShotTimer = UPGRADE_DURATION;
            break;
        case 4:
            piercingBulletsTimer = UPGRADE_DURATION;
            break;
        case 5:
            shieldTimer = UPGRADE_DURATION;
            break;
        case 6:
            speedMultiplier = 1.5f;
            fasterMovementTimer = UPGRADE_DURATION;
            break;
        case 7:
            slowEnemiesTimer = UPGRADE_DURATION;
            break;
    }
}

void Player::UpdateUpgrades() {
    if (fasterShootingTimer > 0) {
        fasterShootingTimer -= GetFrameTime();
        if (fasterShootingTimer <= 0) {
            shootSpeedMultiplier = 1.0f;
        }
    }
    
    if (biggerBulletsTimer > 0) {
        biggerBulletsTimer -= GetFrameTime();
        if (biggerBulletsTimer <= 0) {
            bulletSizeMultiplier = 1.0f;
        }
    }
    
    if (tripleShotTimer > 0) {
        tripleShotTimer -= GetFrameTime();
    }
    
    if (piercingBulletsTimer > 0) {
        piercingBulletsTimer -= GetFrameTime();
    }
    
    if (shieldTimer > 0) {
        shieldTimer -= GetFrameTime();
    }
    
    if (fasterMovementTimer > 0) {
        fasterMovementTimer -= GetFrameTime();
        if (fasterMovementTimer <= 0) {
            speedMultiplier = 1.0f;
        }
    }
    
    if (slowEnemiesTimer > 0) {
        slowEnemiesTimer -= GetFrameTime();
    }
}

void Player::ResetAllUpgrades() {
    shootSpeedMultiplier = 1.0f;
    bulletSizeMultiplier = 1.0f;
    speedMultiplier = 1.0f;
    fasterShootingTimer = 0.0f;
    biggerBulletsTimer = 0.0f;
    tripleShotTimer = 0.0f;
    piercingBulletsTimer = 0.0f;
    shieldTimer = 0.0f;
    fasterMovementTimer = 0.0f;
    slowEnemiesTimer = 0.0f;
}
