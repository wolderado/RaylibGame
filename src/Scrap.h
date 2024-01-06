//
// Created by Salih on 06-Jan-24.
//

#ifndef SRC_SCRAP_H
#define SRC_SCRAP_H

#include "GameObject.h"
#include "Player.h"
#include "ParticleManager.h"

class Player;

class Scrap : public GameObject {
public:
    int rewardAmount = 10;


    Scrap(Player* targetPlayer);
    void Update(float deltaTime) override;
    void Render(float deltaTime) override;
    void OnInit() override;

private:
    const float ScrapMoveSpeed = 0.04f;
    const float ScrapMaxMoveSpeed = 0.4f;
    const float CollectRange = 5.0f;
    const float PullRange = 75.0f;
    const float DriftFadeSpeed = 0.2f;
    const float StartDriftForce = 0.1f;
    const float MaxPullDuration = 5.0f;
    const float IdleDuration = 1.0f;

    Player* collectSource = nullptr;

    bool wasCollected = false;
    bool isMoving = false;
    float pullLineColorTimer = 0;
    float pullTimer = 0;
    float driftTimer = 0;
    float createParticleTimer = 0;
};


#endif //SRC_SCRAP_H
