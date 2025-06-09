#pragma once
#include "Entity.h"
class Bullet : public Entity
{
public:
    float speed;
    bool isActive;
    bool isPiercing = false;
    int piercingHits = 0;
    int maxPiercingHits = 3;

    Bullet(Texture* texture, float posx, float posy, float size, Vector2 dir, float speed, bool piercing = false);
    void Update() override;
    void Draw() const override;

private:
    void CheckBounds();
    Vector2 NormalizeVector2(Vector2 vector);
    Vector2 direction;
};

