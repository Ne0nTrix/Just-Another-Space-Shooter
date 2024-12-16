#pragma once
#include "Entity.h"
class Background : public Entity
{
public:
	Background(Texture* texture, float posx, float posy, float speed);
	void Draw() const override;
	void Update() override;

private:
	float speed;
	const float HEIGHT = 960;
};

