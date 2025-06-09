#pragma once
#include "ComplexEnemy.h"

class HeavyEnemy : public ComplexEnemy {
public:
    int health;
    int maxHealth;
    float damageFlashTimer;
    
    HeavyEnemy(Texture* texture, float posx, float posy, float speed);
    void Update(const std::vector<ComplexEnemy>& allEnemies) override;
    void Draw() const override;
    bool TakeDamage(); // Returns true if enemy is destroyed
    
private:
    const float DAMAGE_FLASH_DURATION = 0.2f;
};