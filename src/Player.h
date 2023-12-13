//
// Created by Salih on 09-Dec-23.
//class

#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
#include <memory>
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"
#include "Globals.h"





using namespace std;

class Player {
public:
    Vector3 Position;
    Vector3 Rotation;

    Player();
    void Init();
    void Update(float deltaTime);
    Camera* GetCamera();
    Vector3 GetVelocityNormalized();
    Vector3 GetVelocity();
    float GetVelocityRatioToMaxValue();

private:
    const float rotateAccelerateSpeed = 0.075f;
    const float rotateSettleSpeed = 8;
    const float maxRotationSpeed = 0.04f;
    const float thrustChangeSpeedMin = 0.1f;
    const float thrustChangeSpeedMax = 1.0f;
    const float stopInertiaSpeed = 4.0f;
    const float maxVelocity = 0.3f;

    Camera3D playerCamera;
    Vector3 smoothedInput;
    Vector3 currentVelocity;



    void ProcessInput(float deltaTime);
    void ProcessRotation(float deltaTime);
    void ProcessThrust(float deltaTime);
};

#endif // PLAYER_H