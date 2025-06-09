#include "HeavyEnemy.h"
#include <cmath>

HeavyEnemy::HeavyEnemy(Texture* texture, float posx, float posy, float speed)
    : ComplexEnemy(texture, posx, posy, speed * 0.7f), health(3), maxHealth(3), damageFlashTimer(0.0f) {
    size = 48;
    this->posx = posx;
    this->posy = posy;
    this->speed = speed * 0.7f;
    this->texture = texture;
    this->isActive = true;
}

void HeavyEnemy::Update(const std::vector<ComplexEnemy>& allEnemies) {
    if (damageFlashTimer > 0) {
        damageFlashTimer -= GetFrameTime();
    }
    ComplexEnemy::Update(allEnemies);
}

void HeavyEnemy::Draw() const {
    if (isActive) {
        Vector2 center = { static_cast<float>(texture->width / 2.0), static_cast<float>(texture->height / 2.0) };
        Color enemyColor = WHITE;
        
        if (damageFlashTimer > 0) {
            enemyColor = RED;
        } else {
            switch (health) {
                case 3:
                    enemyColor = WHITE;
                    break;
                case 2:
                    enemyColor = ORANGE;
                    break;
                case 1:
                    enemyColor = RED;
                    break;
            }
        }
        float scale = size / 32.0f;
        DrawTexturePro(*texture,
            { 0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height) },
            { posx, posy, static_cast<float>(texture->width) * scale, static_cast<float>(texture->height) * scale },
            { center.x * scale, center.y * scale }, rotation, enemyColor);
        float healthBarWidth = size;
        float healthBarHeight = 4;
        float healthBarY = posy - 10;
        DrawRectangle((int)(posx - healthBarWidth/2), (int)healthBarY, (int)healthBarWidth, (int)healthBarHeight, DARKGRAY);
        float healthPercent = (float)health / (float)maxHealth;
        Color healthColor = (health > 1) ? GREEN : RED;
        DrawRectangle((int)(posx - healthBarWidth/2), (int)healthBarY, (int)(healthBarWidth * healthPercent), (int)healthBarHeight, healthColor);
        DrawCircle((int)(posx + 8), (int)(posy + 8), 3, Color{200, 200, 255, 100});
    }
}

bool HeavyEnemy::TakeDamage() {
    health--;
    damageFlashTimer = DAMAGE_FLASH_DURATION;
    if (health <= 0) {
        isActive = false;
        return true;
    }
    return false;
}