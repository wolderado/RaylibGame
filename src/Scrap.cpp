//
// Created by Salih on 06-Jan-24.
//

#include "Scrap.h"

void Scrap::Update(float deltaTime) {
    GameObject::Update(deltaTime);

    if(wasCollected || collectSource == nullptr)
        return;

    float distToSource = Vector3DistanceSqr(Position,collectSource->Position);

    if(isMoving)
    {
        //Moving to be collected
        pullTimer += deltaTime;
        Vector3 targetPos = collectSource->GetCollectPosition();
        Vector3 direction = Vector3Normalize(Vector3Subtract(targetPos,Position));
        currentVelocity = Vector3Add(currentVelocity,Vector3Scale(direction,ScrapMoveSpeed));
        currentVelocity = Vector3ClampValue(currentVelocity,0,ScrapMaxMoveSpeed );

        if(distToSource < CollectRange * CollectRange || pullTimer > MaxPullDuration)
        {
            currentVelocity = Vector3Zero();
            collectSource->AddScrap(rewardAmount);
            Hurt(9999);
            wasCollected = true;
        }
    }
    else
    {
        //Drifting in space
        driftTimer+= deltaTime;
        currentVelocity = Vector3Lerp(currentVelocity,Vector3Zero(),deltaTime * DriftFadeSpeed);
        if(distToSource < PullRange * PullRange && driftTimer > IdleDuration)
        {
            isMoving = true;
        }
    }


    //Particles
    //Distance cull particles
    if(distToSource < PullRange * PullRange * 3.0f) {
        createParticleTimer += deltaTime;
        if (createParticleTimer > 0.075f) {
            createParticleTimer = 0;
            Vector3 glitterPos = Vector3Scale(Utility::GetRandomDirection(), ((float) rewardAmount * 0.2f));
            glitterPos = Vector3Add(glitterPos, Position);
            ParticleManager::GetInstance()->CreateScrapGlitter(glitterPos, PALETTE_PURPLE2);
        }
    }




}

void Scrap::Render(float deltaTime) {
    GameObject::Render(deltaTime);


    Renderer::GetInstance()->RenderScrap(Position,((float)rewardAmount*0.025f));

    if(isMoving)
    {
        pullLineColorTimer += deltaTime * 10;
        float t = abs(sin(pullLineColorTimer));
        Color lineColor = Utility::LerpColor(PALETTE_PURPLE3,Utility::GetZeroAlphaColor(PALETTE_PURPLE3),t);
        DrawLine3D(Position,collectSource->GetCollectPosition(),lineColor);
    }

    //DrawSphereWires(Position,1.0f,6,6,RED);
}

Scrap::Scrap(Player* targetPlayer) {

    collectSource = targetPlayer;
    CanCollide = false;
    IgnoresAllCollisions = true;

}

void Scrap::OnInit() {
    GameObject::OnInit();

    SetHealth(100);
    SetVelocity(Vector3Scale(Utility::GetRandomDirection(), StartDriftForce));
}
