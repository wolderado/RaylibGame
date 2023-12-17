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
#include "raymath.h"


const bool DEBUG_SHOW_GIZMO = false;



class HUD {
public:
    float swayAngleAmount = 5.0f;
    float swayPositionAmount = 30.0f;
    float swaySpeed = 5.0f;

    void Init(shared_ptr<Player> targetPlayer);
    void Render(float deltaTime);
    void Render3D(float deltaTime);
    void Unload();
private:
    shared_ptr<Player> player;
    Texture cockpitTexture;
    Model playerGunModel;
    Vector3 smoothSway;

    void DrawGuns(float deltaTime);
};


#endif //SRC_HUD_H
