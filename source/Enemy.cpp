#include "Enemy.h"

Enemy::Enemy(Texture* texture, float posx, float posy, float speed)
: Entity(texture, posx, posy, 32), isActive(true), speed(speed), direction(1), timeSinceLastShot(0.0f) {
    shootTimer = 0;
    shootInterval = GetRandomValue(2, 5);
}

void Enemy::Update()
{
    if (isActive) {
        posx += direction * speed * GetFrameTime();

        float buffer = 5.0f;
        if (posx <= buffer) {
            posx = buffer;
            direction = 1; 
        }
        else if (posx >= GetScreenWidth() - size - buffer) {
            posx = GetScreenWidth() - size - buffer;
            direction = -1;
        }
    }
}

void Enemy::Draw() const
{
    if (isActive) {
        DrawTexture(*texture, posx, posy, WHITE);
    }
}

bool Enemy::CanShoot(float deltaTime)
{
    shootTimer += deltaTime;
    if (shootTimer >= shootInterval) {
        shootTimer = 0;
        return true;
    }
    return false;
}
