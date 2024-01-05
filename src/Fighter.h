//
// Created by Salih on 02-Jan-24.
//

#ifndef SRC_FIGHTER_H
#define SRC_FIGHTER_H

#include "GameObject.h"
#include "ParticleManager.h"
#include "BulletManager.h"





//Basic FSM AI
//TODO: Implement a class based FSM with Enter,Exit and Process functions (https://www.aleksandrhovhannisyan.com/blog/implementing-a-finite-state-machine-in-cpp/)
enum AIState
{
    Idle,
    FindTarget,
    AttackTarget,
    MoveTarget
};


class Fighter : public GameObject {
public:
    AIState CurrentAIState;

    explicit Fighter(TEAM team, Vector3 position);
    void Update(float deltaTime) override;
    void Render(float deltaTime) override;
    void OnInit() override;
    void Destroy() override;
    void LookForTargets();
    void SetTarget(shared_ptr<GameObject> newTarget);
    void SetMoveTarget(Vector3 position);


private:

    const float RotateSpeedMax = 2.0f;
    const float RotateSpeedMin = 1.0f;
    const float ThrustSpeed = 0.5f;
    const float MaxVelocity = 0.25f;
    const float ShootRange = 100;
    const float ShootCooldown = 0.05f;
    const float ShootCone = 0.8f;
    const float ChangeTargetCooldown = 10.0f;

    float rotateTimer;
    float shootTimer;
    Quaternion lookRot;
    Quaternion smoothRot;
    shared_ptr<GameObject> target;
    Vector3 moveTarget;
    float selectTargetTimer;
    float currentRotateSpeed;
    float sameTargetRotateSpeedBonus;
    int shootCanonIndex = 0;


    void ProcessMoveTarget(float deltaTime,Vector3 targetPos);
    void ProcessShoot(float deltaTime,Vector3 targetPos);

};


#endif //SRC_FIGHTER_H
