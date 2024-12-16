#include "Background.h"

Background::Background(Texture* texture, float posx, float posy, float speed)
: Entity(texture, posx, posy, 0), speed(speed) {}

void Background::Draw() const
{
	DrawTexture(*texture, (int)posx, (int)posy, WHITE);
	DrawTexture(*texture, (int)posx, (int)(posy - HEIGHT), WHITE);
}

void Background::Update()
{
	posy += speed * GetFrameTime();
	if (posy >= HEIGHT)posy = 0;
}
