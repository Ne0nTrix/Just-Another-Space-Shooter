#pragma once
#include "Entity.h"
class Bullet : public Entity
{
public:
	float speed;
	bool isActive;

	Bullet(Texture* texture, float posx, float posy, float size, Vector2 dir, float speed);
	void Update() override;
	void Draw() const override;

private:
	void CheckBounds();
	Vector2 NormalizeVector2(Vector2 vector);
	Vector2 direction;
};

