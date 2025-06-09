#pragma once
#include "Entity.h"

enum class UpgradeType {
    FASTER_SHOOTING,
    BIGGER_BULLETS,
    EXTRA_LIFE,
    TRIPLE_SHOT,           // 3 bullets at once
    PIERCING_BULLETS,      // Bullets go through enemies
    SHIELD,                // Temporary invincibility
    FASTER_MOVEMENT,       // Increased ship speed
    SLOW_ENEMIES          // Slow down all enemies
};

class Upgrade : public Entity {
public:
    bool isActive;
    UpgradeType type;
    float fallSpeed;
    float duration; // For temporary upgrades
    
    Upgrade(Texture* texture, float posx, float posy, UpgradeType upgradeType);
    void Update() override;
    void Draw() const override;
    
private:
    void CheckBounds();
    float blinkTimer; // For visual effect
};