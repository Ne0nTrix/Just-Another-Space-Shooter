#pragma once
#include "raylib.h"
#include "Entity.h"
class Enemy : public Entity
{
public:
    bool isActive;
    float speed;
    float shootTimer;
    Enemy(Texture* texture, float posx, float posy, float speed);
    void Update() override;
    void Draw() const override;
    bool CanShoot(float deltaTime);
protected:
    float shootInterval;
    float timeSinceLastShot;
private:
    float direction;
};

