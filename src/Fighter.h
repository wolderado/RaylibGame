//
// Created by Salih on 02-Jan-24.
//

#ifndef SRC_FIGHTER_H
#define SRC_FIGHTER_H

#include "GameObject.h"
#include "ParticleManager.h"


enum AIState
{
    Idle,
    FindTarget,
    AttackTarget
};


class Fighter : public GameObject {
public:
    const float RotateSpeed = 0.5f;
    const float ThrustSpeed = 0.5f;
    const float MaxVelocity = 0.25f;

    AIState CurrentAIState;

    explicit Fighter(TEAM team, Vector3 position);
    void Update(float deltaTime) override;
    void Render(float deltaTime) override;
    void OnInit() override;
    void Destroy() override;
    void FindANewTarget();
    void SetTarget(shared_ptr<GameObject> newTarget);



private:
    float rotateTimer;
    Quaternion lookRot;
    Quaternion smoothRot;
    shared_ptr<GameObject> target;
    float selectTargetTimer;

};


#endif //SRC_FIGHTER_H
