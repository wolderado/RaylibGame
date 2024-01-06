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
#include "Event.h"


using namespace std;

class Player : public GameObject {
public:
    static Player* GetInstance();
    Player();
    void Init();
    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;
    void Hurt(float damage) override;
    Camera* GetCamera();
    Camera* GetHUDCamera();
    float GetVelocityRatioToMaxValue();
    Vector3 GetCameraDirection();
    Vector3 GetSwayInput();
    void ShakeCamera(float amount);
    void AddScrap(int amount);
    int GetScrap() { return totalScrap; };
    Vector3 GetCollectPosition();
    Event<int> OnScrapGain;
    Event<int> OnShoot;

private:
    static Player* instance;
    const float rotateAccelerateSpeed = 0.075f;
    const float rotateSettleSpeed = 8;
    const float maxRotationSpeed = 0.04f;
    const float thrustChangeSpeedMin = 0.1f;
    const float thrustChangeSpeedMax = 1.0f;
    const float thrustShakeTrauma = 0.2f;
    const float stopInertiaSpeed = 4.0f;
    const float maxVelocity = 0.3f;
    const float shootShakeTrauma = 0.2f;
    const float shootCooldown = STAT_SHOOT_COOLDOWN_PLAYER;
    const float shootBackwardsPush = 0.00035f;
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
    int shootCanonIndex = 0;
    bool shotThisFrame = false;
    int totalScrap = 0;

    void ProcessInput(float deltaTime);
    void ProcessRotation(float deltaTime);
    void ProcessThrust(float deltaTime);
    void ProcessShoot(float deltaTime);
    void ProcessCamera(float deltaTime);
    void OnCollision(GameObject* otherObject,Vector3 collisionTotalVelocity) override;
};

#endif // PLAYER_H