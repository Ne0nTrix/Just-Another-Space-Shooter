#include "LaserTurret.h"
#include <cmath>

LaserTurret::LaserTurret(Texture* texture, float posx, float posy, bool leftSide)
    : Entity(texture, posx, posy, 32), isActive(true), shootTimer(0.0f),
      shootInterval(GetRandomValue(3, 7)), laserDuration(2.0f), laserTimer(0.0f), 
      isShooting(false), isLeftSide(leftSide), initialY(posy), moveSpeed(50.0f), 
      moveRange(100.0f), moveDirection(1) {
}

void LaserTurret::Update() {
    if (isActive) {
        if (!isShooting) {
            posy += moveSpeed * moveDirection * GetFrameTime();
            
            if (posy >= initialY + moveRange) {
                posy = initialY + moveRange;
                moveDirection = -1;
            } else if (posy <= initialY - moveRange) {
                posy = initialY - moveRange;
                moveDirection = 1;
            }
        }
        
        shootTimer += GetFrameTime();
        
        if (isShooting) {
            laserTimer += GetFrameTime();
            if (laserTimer >= laserDuration) {
                StopLaser();
            }
        }
    }
}

void LaserTurret::Draw() const {
    if (isActive) {
        Color turretColor = isShooting ? RED : WHITE;
        
        if (isLeftSide) {
            DrawTexture(*texture, (int)posx, (int)posy, turretColor);
        } else {
            Rectangle sourceRec = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
            Rectangle destRec = { posx, posy, (float)texture->width, (float)texture->height };
            Vector2 origin = { 0, 0 };
            
            sourceRec.width = -(float)texture->width;
            
            DrawTexturePro(*texture, sourceRec, destRec, origin, 0.0f, turretColor);
        }
        
        DrawCircle((int)(posx + 16), (int)(posy + 16), 3, isShooting ? YELLOW : GRAY);
        
        if (isShooting) {
            Rectangle laserRect = GetLaserRect();
            
            float alpha = 0.7f + 0.3f * sin(GetTime() * 20.0f);
            Color laserColor = {255, 50, 50, (unsigned char)(255 * alpha)};
            Color laserGlow = {255, 100, 100, (unsigned char)(100 * alpha)};
            
            DrawRectangle((int)(laserRect.x - 2), (int)laserRect.y, (int)(laserRect.width + 4), (int)laserRect.height, laserGlow);
            
            DrawRectangle((int)laserRect.x, (int)laserRect.y, (int)laserRect.width, (int)laserRect.height, laserColor);
            
            if (isLeftSide) {
                DrawCircle((int)(posx + 32), (int)(posy + 16), 4, Color{255, 255, 100, (unsigned char)(200 * alpha)});
                DrawCircle((int)(posx + 32), (int)(posy + 16), 2, Color{255, 255, 255, (unsigned char)(255 * alpha)});
            } else {
                DrawCircle((int)posx, (int)(posy + 16), 4, Color{255, 255, 100, (unsigned char)(200 * alpha)});
                DrawCircle((int)posx, (int)(posy + 16), 2, Color{255, 255, 255, (unsigned char)(255 * alpha)});
            }
            
            for (int i = 0; i < 5; i++) {
                float sparkX = laserRect.x + GetRandomValue(0, (int)laserRect.width);
                float sparkY = laserRect.y + GetRandomValue(0, (int)laserRect.height);
                DrawCircle((int)sparkX, (int)sparkY, 2, YELLOW);
            }
        }
    }
}

bool LaserTurret::CanShoot(float deltaTime) {
    if (shootTimer >= shootInterval && !isShooting) {
        shootTimer = 0.0f;
        shootInterval = GetRandomValue(4, 8);
        return true;
    }
    return false;
}

void LaserTurret::StartLaser() {
    isShooting = true;
    laserTimer = 0.0f;
}

void LaserTurret::StopLaser() {
    isShooting = false;
    laserTimer = 0.0f;
}

Rectangle LaserTurret::GetLaserRect() const {
    if (isLeftSide) {
        return Rectangle{
            posx + 32,
            posy + 16 - LASER_WIDTH/2,
            GetScreenWidth() - (posx + 32),
            LASER_WIDTH
        };
    } else {
        return Rectangle{
            0,
            posy + 16 - LASER_WIDTH/2,
            posx,
            LASER_WIDTH
        };
    }
}