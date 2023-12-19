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
#include "GameObject.h"


using namespace std;

class Player : public GameObject {
public:
    Player();
    void Init();
    void Update(float deltaTime) override;
    Camera* GetCamera();
    Camera* GetHUDCamera();
    float GetVelocityRatioToMaxValue();
    Vector3 GetCameraDirection();
    Vector3 GetSwayInput();
    void ShakeCamera(float amount);

private:
    const float rotateAccelerateSpeed = 0.075f;
    const float rotateSettleSpeed = 8;
    const float maxRotationSpeed = 0.04f;
    const float thrustChangeSpeedMin = 0.1f;
    const float thrustChangeSpeedMax = 1.0f;
    const float stopInertiaSpeed = 4.0f;
    const float maxVelocity = 0.3f;


    Camera3D playerCamera;
    Camera3D hudCamera;
    Vector3 smoothedInput;
    float thrust;
    float smoothThrust;
    float trauma = 0;
    float traumaDecay = 1.0f;
    float shakeMaxMoveAmount = 0.5f;
    Vector3 camPosition;
    Vector3 shakeOffset;



    void ProcessInput(float deltaTime);
    void ProcessRotation(float deltaTime);
    void ProcessThrust(float deltaTime);
    void ProcessCamera(float deltaTime);
    void OnCollision(GameObject* otherObject,Vector3 collisionTotalVelocity) override;
};

#endif // PLAYER_H