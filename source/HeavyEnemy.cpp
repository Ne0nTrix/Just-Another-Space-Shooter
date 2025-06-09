#include "HeavyEnemy.h"
#include <cmath>

HeavyEnemy::HeavyEnemy(Texture* texture, float posx, float posy, float speed)
    : ComplexEnemy(texture, posx, posy, speed * 0.7f), health(3), maxHealth(3), damageFlashTimer(0.0f) {
    // Make it bigger than regular enemies
    size = 48; // Increase size from default 32
    
    // Make sure all inherited properties are properly set
    this->posx = posx;
    this->posy = posy;
    this->speed = speed * 0.7f; // Heavy enemies are slower
    this->texture = texture;
    this->isActive = true;
}

void HeavyEnemy::Update(const std::vector<ComplexEnemy>& allEnemies) {
    // Update damage flash timer
    if (damageFlashTimer > 0) {
        damageFlashTimer -= GetFrameTime();
    }
    
    // Call parent update
    ComplexEnemy::Update(allEnemies);
}

void HeavyEnemy::Draw() const {
    if (isActive) {
        Vector2 center = { static_cast<float>(texture->width / 2.0), static_cast<float>(texture->height / 2.0) };
        
        // Determine color based on health and damage flash
        Color enemyColor = WHITE;
        
        if (damageFlashTimer > 0) {
            // Flash red when damaged
            enemyColor = RED;
        } else {
            // Color based on health
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
        
        // Draw the enemy with scaling for bigger size
        float scale = size / 32.0f; // Scale based on size difference
        DrawTexturePro(*texture,
            { 0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height) },
            { posx, posy, static_cast<float>(texture->width) * scale, static_cast<float>(texture->height) * scale },
            { center.x * scale, center.y * scale }, rotation, enemyColor);
        
        // Draw health indicator
        float healthBarWidth = size;
        float healthBarHeight = 4;
        float healthBarY = posy - 10;
        
        // Background bar
        DrawRectangle((int)(posx - healthBarWidth/2), (int)healthBarY, (int)healthBarWidth, (int)healthBarHeight, DARKGRAY);
        
        // Health bar
        float healthPercent = (float)health / (float)maxHealth;
        Color healthColor = (health > 1) ? GREEN : RED;
        DrawRectangle((int)(posx - healthBarWidth/2), (int)healthBarY, (int)(healthBarWidth * healthPercent), (int)healthBarHeight, healthColor);
        
        // Add armor effect (metallic shine)
        DrawCircle((int)(posx + 8), (int)(posy + 8), 3, Color{200, 200, 255, 100});
    }
}

bool HeavyEnemy::TakeDamage() {
    health--;
    damageFlashTimer = DAMAGE_FLASH_DURATION;
    
    if (health <= 0) {
        isActive = false;
        return true; // Enemy destroyed
    }
    return false; // Enemy still alive
}