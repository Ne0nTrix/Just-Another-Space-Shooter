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

Bullet::Bullet(Texture* texture, float posx, float posy, float bulletSize, Vector2 dir, float speed, bool piercing)
: Entity(texture, posx, posy, bulletSize), direction(NormalizeVector2(dir)), speed(speed), isActive(true), isPiercing(piercing) {}

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
    if (isActive) {
        float scale = size / texture->width;
        Color bulletColor = WHITE;
        
        // Special visual effect for piercing bullets
        if (isPiercing) {
            bulletColor = Color{255, 200, 100, 255}; // Golden color
            // Add trail effect
            DrawCircle((int)(posx + size/2), (int)(posy + size/2), size/2 + 2, Color{255, 200, 100, 100});
        }
        
        DrawTextureEx(*texture, {posx, posy}, 0.0f, scale, bulletColor);
        
        // Add glow effect for bigger bullets
        if (size > 4) {
            DrawCircle((int)(posx + size/2), (int)(posy + size/2), size/2, Color{255, 255, 100, 50});
        }
    }
}

void Bullet::CheckBounds()
{
    if (posx < 0 || posx > GetScreenWidth() || posy < 0 || posy > GetScreenHeight()) {
        isActive = false;
    }
}

