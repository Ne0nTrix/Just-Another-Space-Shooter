#include "LaserTurret.h"
#include <cmath>

LaserTurret::LaserTurret(Texture* texture, float posx, float posy, bool leftSide)
    : Entity(texture, posx, posy, 32), isActive(true), shootTimer(0.0f), // Changed from 48 to 32
      shootInterval(GetRandomValue(3, 7)), laserDuration(2.0f), laserTimer(0.0f), 
      isShooting(false), isLeftSide(leftSide), initialY(posy), moveSpeed(50.0f), 
      moveRange(100.0f), moveDirection(1) {
}

void LaserTurret::Update() {
    if (isActive) {
        // Only move when not shooting
        if (!isShooting) {
            // Update vertical movement
            posy += moveSpeed * moveDirection * GetFrameTime();
            
            // Check bounds and reverse direction
            if (posy >= initialY + moveRange) {
                posy = initialY + moveRange;
                moveDirection = -1;
            } else if (posy <= initialY - moveRange) {
                posy = initialY - moveRange;
                moveDirection = 1;
            }
        }
        
        // Update shooting timer
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
        // Draw the turret with color based on state
        Color turretColor = isShooting ? RED : WHITE;
        
        if (isLeftSide) {
            // Draw left turret normally (facing right)
            DrawTexture(*texture, (int)posx, (int)posy, turretColor);
        } else {
            // Draw right turret flipped horizontally (facing left)
            Rectangle sourceRec = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
            Rectangle destRec = { posx, posy, (float)texture->width, (float)texture->height };
            Vector2 origin = { 0, 0 };
            
            // Flip horizontally by using negative width
            sourceRec.width = -(float)texture->width;
            
            DrawTexturePro(*texture, sourceRec, destRec, origin, 0.0f, turretColor);
        }
        
        // Draw movement indicator (centered on 32x32 sprite)
        DrawCircle((int)(posx + 16), (int)(posy + 16), 3, isShooting ? YELLOW : GRAY);
        
        // Draw laser beam when shooting
        if (isShooting) {
            Rectangle laserRect = GetLaserRect();
            
            // Draw laser with pulsing effect
            float alpha = 0.7f + 0.3f * sin(GetTime() * 20.0f);
            Color laserColor = {255, 50, 50, (unsigned char)(255 * alpha)};
            Color laserGlow = {255, 100, 100, (unsigned char)(100 * alpha)};
            
            // Draw glow effect
            DrawRectangle((int)(laserRect.x - 2), (int)laserRect.y, (int)(laserRect.width + 4), (int)laserRect.height, laserGlow);
            
            // Draw main laser
            DrawRectangle((int)laserRect.x, (int)laserRect.y, (int)laserRect.width, (int)laserRect.height, laserColor);
            
            // Add muzzle flash at laser origin
            if (isLeftSide) {
                // Muzzle flash at right edge of left turret
                DrawCircle((int)(posx + 32), (int)(posy + 16), 4, Color{255, 255, 100, (unsigned char)(200 * alpha)});
                DrawCircle((int)(posx + 32), (int)(posy + 16), 2, Color{255, 255, 255, (unsigned char)(255 * alpha)});
            } else {
                // Muzzle flash at left edge of right turret
                DrawCircle((int)posx, (int)(posy + 16), 4, Color{255, 255, 100, (unsigned char)(200 * alpha)});
                DrawCircle((int)posx, (int)(posy + 16), 2, Color{255, 255, 255, (unsigned char)(255 * alpha)});
            }
            
            // Add some particles/sparks
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
        shootInterval = GetRandomValue(4, 8); // Random interval between shots
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
        // Laser goes from RIGHT side of left turret to right edge of screen
        return Rectangle{
            posx + 32,  // Right edge of the 32x32 sprite
            posy + 16 - LASER_WIDTH/2,  // Middle height of 32x32 sprite
            GetScreenWidth() - (posx + 32),  // Width from right edge to screen edge
            LASER_WIDTH
        };
    } else {
        // Laser goes from left edge of screen to LEFT side of right turret  
        return Rectangle{
            0,  // From left edge of screen
            posy + 16 - LASER_WIDTH/2,  // Middle height of 32x32 sprite
            posx,  // To left edge of right turret sprite
            LASER_WIDTH
        };
    }
}