#include "Entity.h"


Entity::Entity(Texture* texture, float posx, float posy, float size)
{
	this->texture = texture;
	this->posx = posx;
	this->posy = posy;
	this->size = size;
}

void Entity::Draw() const
{
	DrawTexture(*texture, posx, posy, WHITE);
}


void Entity::Update()
{
}


