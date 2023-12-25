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
#include "BulletManager.h"


using namespace std;

class Player : public GameObject {
public:
    using ShootDelegate = function<void(int)>;

    Player();
    void Init();
    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;
    Camera* GetCamera();
    Camera* GetHUDCamera();
    float GetVelocityRatioToMaxValue();
    Vector3 GetCameraDirection();
    Vector3 GetSwayInput();
    void ShakeCamera(float amount);
    void SetShootDelegate(const function<void(int)> &delegate);
    static Player* GetInstance();


private:
     float rotateAccelerateSpeed = 0.075f;
     float rotateSettleSpeed = 8;
     float maxRotationSpeed = 0.04f;
     float thrustChangeSpeedMin = 0.1f;
     float thrustChangeSpeedMax = 1.0f;
     float thrustShakeTrauma = 0.2f;
     float stopInertiaSpeed = 4.0f;
     float maxVelocity = 0.3f;
     float shootShakeTrauma = 0.2f;
     float shootCooldown = 0.05f;
     float shootBackwardsPush = 0.00035f;
     const float thrustFOVChange = 7.0f;
     const float shootFOVChange = -10.0f;
     const float defaultFOV = 60.0f;
     const float fovChangeSpeed = 10.0f;

    Camera3D playerCamera;
    Camera3D hudCamera;
    Vector3 smoothedInput;
    float thrust = 0;
    float smoothThrust = 0;
    float trauma = 0;
    float traumaDecay = 1.0f;
    float shakeMaxMoveAmount = 0.5f;
    float shootTimer = 0;
    Vector3 camPosition = {0,0,0};
    Vector3 shakeOffset = {0,0,0};
    ShootDelegate shootDelegate;
    int shootCanonIndex = 0;
    bool shotThisFrame = false;
    static Player* instance;

    void ProcessInput(float deltaTime);
    void ProcessRotation(float deltaTime);
    void ProcessThrust(float deltaTime);
    void ProcessShoot(float deltaTime);
    void ProcessCamera(float deltaTime);
    void OnCollision(GameObject* otherObject,Vector3 collisionTotalVelocity) override;
};

#endif // PLAYER_H