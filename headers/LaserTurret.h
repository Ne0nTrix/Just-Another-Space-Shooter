#pragma once
#include "Entity.h"
#include "raylib.h"

class LaserTurret : public Entity {
public:
    bool isActive;
    float shootTimer;
    float shootInterval;
    float laserDuration;
    float laserTimer;
    bool isShooting;
    bool isLeftSide;
    
    float initialY;
    float moveSpeed;
    float moveRange;
    int moveDirection;
    
    LaserTurret(Texture* texture, float posx, float posy, bool leftSide);
    void Update() override;
    void Draw() const override;
    bool CanShoot(float deltaTime);
    void StartLaser();
    void StopLaser();
    Rectangle GetLaserRect() const;

private:
    const float LASER_WIDTH = 8.0f;
};