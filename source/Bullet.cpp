#include "Bullet.h"
#include "raylib.h"
#include <cmath>

Vector2 Bullet::NormalizeVector2(Vector2 vector)
{
    float length = sqrt(vector.x * vector.x + vector.y * vector.y);
    if (length != 0) {
        return { vector.x / length, vector.y / length };
    }
    else {
        return { 0, 0 };
    }
}

Bullet::Bullet(Texture* texture, float posx, float posy, float size, Vector2 dir, float speed)
: Entity(texture, posx, posy, 4), direction(NormalizeVector2(dir)), speed(speed), isActive(true) {}

void Bullet::Update()
{
    if (isActive) {
        posx += direction.x * speed * GetFrameTime();
        posy += direction.y * speed * GetFrameTime();

        CheckBounds();
    }
}

void Bullet::Draw() const
{
    if (isActive)DrawTexture(*texture, posx, posy, WHITE);
}

void Bullet::CheckBounds()
{
    if (posx < 0 || posx > GetScreenWidth() || posy < 0 || posy > GetScreenHeight()) {
        isActive = false;
    }
}

