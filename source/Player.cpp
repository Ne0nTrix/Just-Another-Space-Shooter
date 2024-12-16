#include "Player.h"
#include "Bullet.h"
#include <cmath>
#include <vector>
Player::Player(Texture* texture, float posx, float posy, float speed)
: Entity(texture, posx, posy, 32), speed(speed), shootCooldown(0.5f), currentCooldown(0.0f), rotation(0.0f) {}

void Player::Event(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed)
{
    if (IsKeyDown(KEY_W)) posy -= speed * GetFrameTime();
    if (IsKeyDown(KEY_S)) posy += speed * GetFrameTime();
    if (IsKeyDown(KEY_A)) posx -= speed * GetFrameTime();
    if (IsKeyDown(KEY_D)) posx += speed * GetFrameTime();
    if (currentCooldown > 0.0f) {
        currentCooldown -= GetFrameTime();
    }

    if (currentCooldown <= 0.0f && IsKeyPressed(KEY_SPACE)) {
        Shoot(bullets, bulletTexture, bulletSpeed);
        currentCooldown = shootCooldown;
    }
    RotateTowards(GetMousePosition());
}

void Player::Draw() const
{
    Vector2 center = { static_cast<float>(texture->width / 2.0), static_cast<float>(texture->height / 2.0) };
    DrawTexturePro(*texture, { 0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height) },
        { posx, posy, static_cast<float>(texture->width), static_cast<float>(texture->height)}, center, rotation, WHITE);
}

void Player::Update()
{
    if (posx < texture->width / 2) posx = texture->width / 2;
    if (posx > GetScreenWidth() - texture->width/2) posx = GetScreenWidth() - texture->width/2;
    if (posy < texture->height / 2) posy = texture->height / 2;
    if (posy > GetScreenHeight() - texture->height/2) posy = GetScreenHeight() - texture->height/2;
}

void Player::RotateTowards(Vector2 target)
{
    float deltaX = target.x - posx;
    float deltaY = target.y - posy;
    rotation = atan2(deltaY, deltaX) * (180 / PI);
    rotation += 90;
}

void Player::Shoot(std::vector<Bullet>& bullets, Texture* bulletTexture, float bulletSpeed)
{
    float radians = (rotation - 90) * (PI / 180);
    Vector2 direction = { cos(radians), sin(radians) };
    bullets.emplace_back(bulletTexture, posx, posy, 4, direction, bulletSpeed);
}
