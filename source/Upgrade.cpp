#include "Upgrade.h"
#include <cmath>

Upgrade::Upgrade(Texture* texture, float posx, float posy, UpgradeType upgradeType)
    : Entity(texture, posx, posy, 16), isActive(true), type(upgradeType), 
      fallSpeed(100.0f), duration(15.0f), blinkTimer(0.0f) {
}

void Upgrade::Update() {
    if (isActive) {
        posy += fallSpeed * GetFrameTime();
        blinkTimer += GetFrameTime();
        CheckBounds();
    }
}

void Upgrade::Draw() const {
    if (isActive) {
        // Create a blinking effect
        float alpha = 0.7f + 0.3f * sin(blinkTimer * 8.0f);
        Color tint = WHITE;
        tint.a = (unsigned char)(255 * alpha);
        
        // Different colors for different upgrade types
        switch (type) {
            case UpgradeType::FASTER_SHOOTING:
                tint.r = 255; tint.g = 100; tint.b = 100; // Red tint
                break;
            case UpgradeType::BIGGER_BULLETS:
                tint.r = 100; tint.g = 100; tint.b = 255; // Blue tint
                break;
            case UpgradeType::EXTRA_LIFE:
                tint.r = 100; tint.g = 255; tint.b = 100; // Green tint
                break;
            case UpgradeType::TRIPLE_SHOT:
                tint.r = 255; tint.g = 255; tint.b = 100; // Yellow tint
                break;
            case UpgradeType::PIERCING_BULLETS:
                tint.r = 255; tint.g = 100; tint.b = 255; // Magenta tint
                break;
            case UpgradeType::SHIELD:
                tint.r = 100; tint.g = 255; tint.b = 255; // Cyan tint
                break;
            case UpgradeType::FASTER_MOVEMENT:
                tint.r = 255; tint.g = 255; tint.b = 255; // White tint
                break;
            case UpgradeType::SLOW_ENEMIES:
                tint.r = 128; tint.g = 128; tint.b = 255; // Light blue tint
                break;
        }
        
        DrawTexture(*texture, (int)posx, (int)posy, tint);
        
        // Draw a glow effect
        DrawCircle((int)(posx + size/2), (int)(posy + size/2), size/2 + 2, 
                  Color{tint.r, tint.g, tint.b, (unsigned char)(50 * alpha)});
    }
}

void Upgrade::CheckBounds() {
    if (posy > GetScreenHeight() + size) {
        isActive = false;
    }
}