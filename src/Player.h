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
#include "SoundManager.h"


using namespace std;

enum PLAYER_UPGRADE_TYPE
{
    None,
    Damage,
    Health,
    Speed
};

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
    void SetCanRotate(bool value) { canRotate = value; };
    void SetCanMove(bool value) { canMove = value; };
    void UpgradeStat(PLAYER_UPGRADE_TYPE upgradeType);
    Event<int> OnScrapGain;
    Event<int> OnShoot;
    Event<void> OnPlayerDeath;


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
    const float shootBackwardsPush = 0.0002f;
    const float thrustFOVChange = 7.0f;
    const float shootFOVChange = -10.0f;
    const float defaultFOV = 60.0f;
    const float fovChangeSpeed = 10.0f;

    float upgradedDamage = 0;
    float upgradedHealth = 0;
    float upgradedSpeed = 0;

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
    bool canRotate = true;
    bool canMove = true;
    int shootSoundPlayCounter = 0;
    int shootPanCounter = 0;
    bool wasPlayingThrustSound = false;


    void ProcessInput(float deltaTime);
    void ProcessRotation(float deltaTime);
    void ProcessThrust(float deltaTime);
    void ProcessShoot(float deltaTime);
    void ProcessCamera(float deltaTime);
    void OnCollision(GameObject* otherObject,Vector3 collisionTotalVelocity) override;
};

#endif // PLAYER_H