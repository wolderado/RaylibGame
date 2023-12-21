//
// Created by Salih on 16-Dec-23.
//

#ifndef SRC_HUD_H
#define SRC_HUD_H
#include <iostream>
#include <cmath>

#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"
#include "Color.h"
#include "Globals.h"
#include "Renderer.h"
#include "Player.h"
#include "World.h"


const bool DEBUG_SHOW_GIZMO = false;



class HUD {
public:
    const float swayAngleAmount = 5.0f;
    const float swayPositionAmount = 30.0f;
    const float swaySpeed = 5.0f;
    const float NozzleReturnToNormalSpeed = 10.0f;
    const float shootCannonOffsetAmount = -2.0f;

    void Init(shared_ptr<Player> targetPlayer);
    void Render(float deltaTime);
    void Render3D(float deltaTime);
    void Unload();
    void GunShoot(int cannonID);

private:
    shared_ptr<Player> player;
    Texture cockpitTexture;
    Model playerGunModel;
    Model playerGunMuzzleModel;
    Vector3 smoothSway;
    Vector3 leftGunShootOffset = {0,0,0};
    Vector3 rightGunShootOffset = {0,0,0};

    void DrawGuns(float deltaTime);

};


#endif //SRC_HUD_H
