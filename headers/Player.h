#pragma once
#include "Entity.h"
#include <vector>
#include "Bullet.h"
class Bullet;
class Player : public Entity
{
public:
	Player(Texture* texture, float posx, float posy, float speed);
	void Event(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed);
	void Draw() const override;
	void Update() override;
	void RotateTowards(Vector2 target);
	void Shoot(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed);
	int GetHealth() const { return health; }
	void SetHealth(int value) { health = value; }

private:
	float speed;
	float rotation;
	float shootCooldown;
	float currentCooldown;
	int health = 3;
};

