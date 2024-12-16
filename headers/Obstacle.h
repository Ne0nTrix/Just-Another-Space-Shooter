#pragma once
#include "Entity.h"

class Obstacle : public Entity
{
public:
	float speed;
	bool isActive;
	int size;
	Obstacle(Texture* texture, float size);
	void Update() override;
	void Draw() const override;
private:
	void CheckBounds();
	Vector2 direction;
};

