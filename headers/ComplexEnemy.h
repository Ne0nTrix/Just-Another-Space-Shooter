#pragma once
#include "Enemy.h"
#include "Bullet.h"
#include <vector>
class ComplexEnemy : public Enemy
{
public:
    float posx, posy, speed, rotation;
    
    ComplexEnemy(Texture* tex, float x, float y, float spd);

    void Update(const std::vector<ComplexEnemy>& allEnemies);
    void Draw() const override;
    void SetPlayerPosition(Vector2 playerPos);
    bool CanShoot(float deltaTime);

private:
    Vector2 targetPos;
    float minDistSq = 1000.0f;
    float repulsionStrength = 200.0f;
};

