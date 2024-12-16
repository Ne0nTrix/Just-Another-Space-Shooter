#pragma once
#include "raylib.h"
class Entity
{
public:
	Texture* texture;
	float posx;
	float posy;
	float size;

	Entity(Texture* texture, float posx, float posy, float size);

	virtual void Draw() const;
	virtual void Update();
};

