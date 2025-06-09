#pragma once
#include "Entity.h"

enum class UpgradeType {
    FASTER_SHOOTING,
    BIGGER_BULLETS,
    EXTRA_LIFE,
    TRIPLE_SHOT,
    PIERCING_BULLETS,
    SHIELD,
    FASTER_MOVEMENT,
    SLOW_ENEMIES
};

class Upgrade : public Entity {
public:
    bool isActive;
    UpgradeType type;
    float fallSpeed;
    float duration;
    
    Upgrade(Texture* texture, float posx, float posy, UpgradeType upgradeType);
    void Update() override;
    void Draw() const override;
    
private:
    void CheckBounds();
    float blinkTimer;
};