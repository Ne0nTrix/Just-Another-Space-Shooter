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
    int GetMaxHealth() const { return maxHealth; }
    
    // Upgrade system
    void ApplyUpgrade(int upgradeType);
    void UpdateUpgrades();
    float GetCurrentShootCooldown() const { return shootCooldown * shootSpeedMultiplier; }
    float GetCurrentBulletSize() const { return bulletSizeMultiplier; }
    float GetCurrentSpeed() const { return speed * speedMultiplier; }
    bool IsShielded() const { return shieldTimer > 0; }
    bool HasTripleShot() const { return tripleShotTimer > 0; }
    bool HasPiercingBullets() const { return piercingBulletsTimer > 0; }
    bool HasSlowEnemies() const { return slowEnemiesTimer > 0; }
	void ResetAllUpgrades();

private:
    float speed;
    float rotation;
    float shootCooldown;
    float currentCooldown;
    int health = 3;
    int maxHealth = 5;
    
    // Upgrade effects
    float shootSpeedMultiplier = 1.0f;
    float bulletSizeMultiplier = 1.0f;
    float speedMultiplier = 1.0f;
    float fasterShootingTimer = 0.0f;
    float biggerBulletsTimer = 0.0f;
    float tripleShotTimer = 0.0f;
    float piercingBulletsTimer = 0.0f;
    float shieldTimer = 0.0f;
    float fasterMovementTimer = 0.0f;
    float slowEnemiesTimer = 0.0f;
    const float UPGRADE_DURATION = 15.0f;
    
    void ShootTriple(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed);
};

