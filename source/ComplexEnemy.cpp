#include "ComplexEnemy.h"
#include <iostream>
#include "Bullet.h"
#include <cmath>


ComplexEnemy::ComplexEnemy(Texture* tex, float posx, float posy, float spd)
    : Enemy(tex, posx, posy, spd), rotation(0), targetPos{ 0, 0 } {
    speed = spd;
    this->posx = posx;
    this->posy = posy;

}

void ComplexEnemy::Update(const std::vector<ComplexEnemy>& allEnemies) {
    Vector2 directionToPlayer = { targetPos.x - posx, targetPos.y - posy };
    float length = sqrt(directionToPlayer.x * directionToPlayer.x +
        directionToPlayer.y * directionToPlayer.y);

    if (length != 0) {
        directionToPlayer.x /= length;
        directionToPlayer.y /= length;
    }

    Vector2 repulsionForce = { 0.0f, 0.0f };
    bool isNearOtherEnemy = false;

    for (const ComplexEnemy& otherEnemy : allEnemies) {
        if (&otherEnemy == this) continue;

        float dx = posx - otherEnemy.posx;
        float dy = posy - otherEnemy.posy;
        float distanceSq = dx * dx + dy * dy;

        if (distanceSq < minDistSq) {
            isNearOtherEnemy = true;

            float distance = sqrt(distanceSq);
            if (distance > 0) {
                float strength = repulsionStrength * (1.0f - distance / sqrt(minDistSq));
                strength = fmax(strength, repulsionStrength * 2.0f);
                repulsionForce.x += dx / distance * strength;
                repulsionForce.y += dy / distance * strength;
            }
        }
    }

    Vector2 finalDirection = { 0.0f, 0.0f };
    if (isNearOtherEnemy) {
        finalDirection = repulsionForce;
    }
    else {
        finalDirection.x = directionToPlayer.x + repulsionForce.x;
        finalDirection.y = directionToPlayer.y + repulsionForce.y;
    }

    float finalLength = sqrt(finalDirection.x * finalDirection.x +
        finalDirection.y * finalDirection.y);
    if (finalLength != 0) {
        finalDirection.x /= finalLength;
        finalDirection.y /= finalLength;
    }

    posx += finalDirection.x * speed * GetFrameTime();
    posy += finalDirection.y * speed * GetFrameTime();

    rotation = atan2(finalDirection.y, finalDirection.x) * RAD2DEG - 90;
}

void ComplexEnemy::Draw() const {
        Vector2 center = { static_cast<float>(texture->width / 2.0), static_cast<float>(texture->height / 2.0) };
        DrawTexturePro(*texture,
            { 0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height) },
            { posx, posy, static_cast<float>(texture->width), static_cast<float>(texture->height) },
            center, rotation, WHITE);
}

void ComplexEnemy::SetPlayerPosition(Vector2 playerPos)
{
    targetPos = playerPos;
}

bool ComplexEnemy::CanShoot(float deltaTime)
{
    shootTimer += deltaTime * 3;
    if (shootTimer >= shootInterval) {
        shootTimer = 0;
        return true;
    }
    return false;
}
