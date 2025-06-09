#include "Obstacle.h"
#include <iostream>
#include <cmath>

Obstacle::Obstacle(Texture* texture, float size)
    : Entity(texture, 0, 0, size), size(size), isActive(true), speed(GetRandomValue(100, 200)) {
    posx = static_cast<float>(GetRandomValue(0, GetScreenWidth() - static_cast<int>(size)));
    posy = static_cast<float>(GetRandomValue(0, GetScreenHeight()/2 - static_cast<int>(size)));
    do {
        direction = { static_cast<float>(GetRandomValue(-1, 1)), static_cast<float>(GetRandomValue(-1, 1)) };
    } while (direction.x == 0 && direction.y == 0);

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

}

void Obstacle::Update()
{
    if (isActive) {
        posx += direction.x * speed * GetFrameTime();
        posy += direction.y * speed * GetFrameTime();
        CheckBounds();
    }
}

void Obstacle::Draw() const
{
    if (isActive) {
        DrawTextureEx(*texture, { posx, posy }, 0.0f, static_cast<float>(size) / texture->width, WHITE);
    }
}

void Obstacle::CheckBounds()
{
    if (posx < -size) {
        posx = GetScreenWidth() + size;
    }
    else if (posx > GetScreenWidth() + size) {
        posx = -size;
    }

    if (posy < -size) {
        posy = GetScreenHeight() + size;
    }
    else if (posy > GetScreenHeight() + size) {
        posy = -size;
    }
}
