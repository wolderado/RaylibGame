//
// Created by Salih on 02-Jan-24.
//

#include "Fighter.h"



void Fighter::Update(float deltaTime) {

    GameObject::Update(deltaTime);


    Vector3 targetPos = Vector3Zero();



    selectTargetTimer += deltaTime;
    float changeTargetTimerRatio = selectTargetTimer / ChangeTargetCooldown;
    if(selectTargetTimer > ChangeTargetCooldown)
    {
        sameTargetRotateSpeedBonus = 1;
        selectTargetTimer = 0;
        LookForTargets();
        return;
    }


    if(CurrentAIState == AttackTarget) {
        if (target == nullptr || target->GetHealth() <= 0) {
            LookForTargets();
            return;
        }

        //DrawLine3D(Position, target->Position, RED);
        Vector3 predictedPos = target->Position;
        //This is highly dependent on bullet speed
        predictedPos = Vector3Add(predictedPos, Vector3Scale(target->GetVelocity(), 100.0f));
        targetPos = predictedPos;

/*        targetPos = target->Position;*/

        //DrawSphereWires(predictedPos, 1, 6, 6, PURPLE);


    }



    if(CurrentAIState == MoveTarget)
    {
        targetPos = moveTarget;
        //DrawLine3D(Position, targetPos, RED);
        //DrawLine3D(Position, target->Position, RED);
    }

    if(CurrentAIState == MoveTarget || CurrentAIState == AttackTarget)
    {
        //Fix for orbiting behaviour. Gradually increase rotate speed when chasing the same target
        sameTargetRotateSpeedBonus = 1.0f + ((changeTargetTimerRatio * changeTargetTimerRatio) * 2.0f);


        ProcessMoveTarget(deltaTime,targetPos);
    }
    else
        sameTargetRotateSpeedBonus = 1;

    currentVelocity = Vector3ClampValue(currentVelocity, -MaxVelocity, MaxVelocity);

    smoothRot = QuaternionSlerp(smoothRot, lookRot, deltaTime * currentRotateSpeed * sameTargetRotateSpeedBonus);
    Rotation = smoothRot;




    //DrawLine3D(Position, Vector3Add(Position,Vector3Scale(forward,10)), PURPLE);
}


void Fighter::Destroy() {
    GameObject::Destroy();

    ParticleManager::GetInstance()->CreateAsteroidExplosion(Position,50);
}

Fighter::Fighter(TEAM team, Vector3 position) : GameObject() {

    myTeam = team;

    if(myTeam == TEAM_ALLY)
        Name = "Ally Fighter";
    else
        Name = "Enemy Fighter";

    Position = position;
    LookForTargets();
    selectTargetTimer = GetRandomValue(0,ChangeTargetCooldown * 100) * 0.01f;

}

void Fighter::Render(float deltaTime) {
    GameObject::Render(deltaTime);


/*    DrawLine3D(Position, Vector3Add(Position, (Vector3){10,0,0}), RED);
    DrawLine3D(Position, Vector3Add(Position, (Vector3){0,10,0}), YELLOW);
    DrawLine3D(Position, Vector3Add(Position, (Vector3){0,0,10}), BLUE);

    DrawLine3D(Position, Vector3Add(Position, Vector3Scale(Vector3Normalize(currentVelocity), 10)), GREEN);*/

    if(DEBUG_SHOW_SHOOT_RANGE)
    {
        DrawSphereWires(Position,ShootRange,6,6,GRAY);
    }
}

void Fighter::OnInit() {
    GameObject::OnInit();

    currentVelocity = Vector3Scale(Utility::GetRandomDirection(),0.001f);
    CollisionSize = 3.0f;
    CanCollide = false;
    currentRotateSpeed = (float)GetRandomValue(RotateSpeedMin * 100.0f,RotateSpeedMax * 100.0f) * 0.01f;
}

void Fighter::LookForTargets() {
    sameTargetRotateSpeedBonus = 1;
    CurrentAIState = FindTarget;
}

void Fighter::SetTarget(shared_ptr<GameObject> newTarget) {
    CurrentAIState = AttackTarget;
    target = shared_ptr<GameObject>(newTarget);
    //cout << "Target acquired: " << target->Name << endl;
    sameTargetRotateSpeedBonus = 1;
    selectTargetTimer = 0;
    shootTimer = 0;
}

void Fighter::SetMoveTarget(Vector3 position) {

    CurrentAIState = MoveTarget;
    //target = nullptr;
    moveTarget = position;
    sameTargetRotateSpeedBonus = 1;
    selectTargetTimer = 0;

}

void Fighter::ProcessMoveTarget(float deltaTime,Vector3 targetPos) {

    //DrawLine3D(Position, target->Position, RED);


    Vector3 toTarget = Vector3Subtract(targetPos, Position);
    float distToTarget = Vector3Length(toTarget);

    //Shoot System
    if (CurrentAIState == AttackTarget) {
        if (distToTarget < ShootRange) {
            shootTimer += deltaTime;
            if (shootTimer > ShootCooldown) {
                shootTimer = 0;
                Vector3 shootDirection = GetForward();
                float dot = Vector3DotProduct(Vector3Normalize(toTarget), shootDirection);
                if (Vector3DotProduct(shootDirection, Vector3Normalize(toTarget)) > ShootCone) {
                    BulletManager::GetInstance()->CreateBullet(Position, shootDirection, 0, myTeam);
                }
            }
        }
    }

/*
    DrawLine3D(Position, Vector3Add(Position, Vector3Scale(GetForward(),10)), GREEN);
    DrawLine3D(Position, Vector3Add(Vector3Add(Position,(Vector3){0,2,0}), Vector3Add(Vector3Add(Position,(Vector3){0,2,0}),Vector3Scale(Vector3Normalize(toTarget),10))), YELLOW);
*/


    if(Utility::IsInsideMapArea(Position) == false)
    {
        if(CurrentAIState == AttackTarget)
        {
            selectTargetTimer = GetRandomValue(0,ChangeTargetCooldown * 50) * 0.01f;
        }

        SetMoveTarget((Vector3){0,100,0});


    }

    //DrawLine3D(Position, targetPos, PURPLE);

    lookRot = Utility::LookAt(Position, targetPos);
    Vector3 thrust = Vector3Scale(GetForward(), ThrustSpeed * deltaTime);
    currentVelocity = Vector3Add(currentVelocity, thrust);
}
