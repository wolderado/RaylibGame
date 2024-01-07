//
// Created by Salih on 02-Jan-24.
//

#include "Fighter.h"



void Fighter::Update(float deltaTime) {




    GameObject::Update(deltaTime);



/*
    Vector3 forward = GetForward();
    Vector3 right = GetRight();
    Vector3 up = GetUp();

    DrawLine3D(Position, Vector3Add(Position,Vector3Scale(forward,10)), BLUE);
    DrawLine3D(Position, Vector3Add(Position,Vector3Scale(right,10)), RED);
    DrawLine3D(Position, Vector3Add(Position,Vector3Scale(up,10)), YELLOW);
*/


    if(DEBUG_DISABLE_AI)
        return;

    Vector3 targetPos = Vector3Zero();



    //Change targets
    selectTargetTimer += deltaTime;
    float changeTargetTimerRatio = selectTargetTimer / ChangeTargetCooldown;
    if(selectTargetTimer > ChangeTargetCooldown)
    {
        sameTargetRotateSpeedBonus = 1;
        selectTargetTimer = 0;
        LookForTargets();
    }

    //Making sure the action is happening inside the map
    if(Utility::IsInsideMapArea(Position) == false)
    {
        if(CurrentAIState == AttackTarget)
        {
            selectTargetTimer = GetRandomValue(0,ChangeTargetCooldown * 50) * 0.01f;
        }

        SetMoveTarget((Vector3){0,100,0});
    }

    if(CurrentAIState == Idle || CurrentAIState == FindTarget)
        currentVelocity = Vector3Lerp(currentVelocity, Vector3Zero(), deltaTime * 0.1f);

    if(CurrentAIState == AttackTarget) {
        if (target == nullptr || target->GetHealth() <= 0) {
            LookForTargets();
            return;
        }



        if (target->GetTeam() == TEAM_PLAYER)
            //Worse targeting for player
            targetPos = target->Position;
        else {
            //Target prediction
            Vector3 predictedPos = target->Position;
            //This is highly dependent on bullet speed
            predictedPos = Vector3Add(predictedPos, Vector3Scale(target->GetVelocity(), 100.0f));
            targetPos = predictedPos;
        }
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

    if(sameTargetRotateSpeedBonus < 1.0f)
        sameTargetRotateSpeedBonus = 1.0f;

    currentVelocity = Vector3ClampValue(currentVelocity, -MaxVelocity, MaxVelocity);
    smoothRot = QuaternionSlerp(smoothRot, lookRot, deltaTime * currentRotateSpeed * sameTargetRotateSpeedBonus);
    Rotation = smoothRot;


    if(DEBUG_SHOW_FIGHTER_TARGETS)
        DrawLine3D(Position, targetPos, RED);
}


void Fighter::Destroy() {
    GameObject::Destroy();

    ParticleManager::GetInstance()->CreateShipExplosion(Position,myTeam);
}

Fighter::Fighter(TEAM team, Vector3 position)  {

    myTeam = team;

    if(myTeam == TEAM_ALLY)
        Name = "Ally Fighter";
    else
        Name = "Enemy Fighter";

    Position = position;
    LookForTargets();
    selectTargetTimer = GetRandomValue(0,ChangeTargetCooldown * 100) * 0.01f;
    Scale = Vector3Scale(Vector3One(), 0.5f);
    lookRot = QuaternionIdentity();
    smoothRot = QuaternionIdentity();

    CurrentAIState = FindTarget;
    shootTimer = 0;
    shootCanonIndex = 0;

}

void Fighter::Render(float deltaTime) {
    GameObject::Render(deltaTime);


/*    DrawLine3D(Position, Vector3Add(Position, (Vector3){10,0,0}), RED);
    DrawLine3D(Position, Vector3Add(Position, (Vector3){0,10,0}), YELLOW);
    DrawLine3D(Position, Vector3Add(Position, (Vector3){0,0,10}), BLUE);

    DrawLine3D(Position, Vector3Add(Position, Vector3Scale(Vector3Normalize(currentVelocity), 10)), GREEN);*/

    if(DEBUG_SHOW_SHOOT_RANGE)
        DrawSphereWires(Position,ShootRange,6,6,GRAY);
}

void Fighter::OnInit() {
    GameObject::OnInit();

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

    //Shoot System
    if (CurrentAIState == AttackTarget) {
        ProcessShoot(deltaTime, targetPos);
    }

    if(Vector3Equals(Position,targetPos))
        return;

    lookRot = Utility::LookAt(Position, targetPos);

    //NAN detection for debugging cause it breaks the whole game if rotation is nan
    if(lookRot.x == NAN) {
        cout << "ERROR: NAN ROTATION DETECTED! " << endl;
        cout << " RAW OBJECT DATA: " << lookRot.x << " " << lookRot.y << " " << lookRot.z << " " << lookRot.w << endl;
        cout << " POS: " << Position.x << " " << Position.y << " " << Position.z << ", TARGET POS:" << targetPos.x
             << " " << targetPos.y << " " << targetPos.z << endl;
        cout << "FIXING NAN.." << endl;
        lookRot = QuaternionIdentity();
    }


    Vector3 thrust = Vector3Scale(GetForward(), ThrustSpeed * deltaTime);
    currentVelocity = Vector3Add(currentVelocity, thrust);
}


void Fighter::ProcessShoot(float deltaTime, Vector3 targetPos) {

    Vector3 toTarget = Vector3Subtract(targetPos, Position);
    float distToTarget = Vector3LengthSqr(toTarget);
    if (distToTarget < ShootRange * ShootRange) {
        shootTimer += deltaTime;
        if (shootTimer > ShootCooldown) {
            shootTimer = 0;
            Vector3 shootDirection = GetForward();
            float dot = Vector3DotProduct(Vector3Normalize(toTarget), shootDirection);
            if (Vector3DotProduct(shootDirection, Vector3Normalize(toTarget)) > ShootCone) {

                shootCanonIndex++;
                if(shootCanonIndex > 1)
                    shootCanonIndex = 0;

                Vector3 forward = GetForward();
                Vector3 right = GetRight();
                Vector3 up = GetUp();

                Vector3 shootPosition = Position;
                shootPosition = Vector3Add(shootPosition, Vector3Scale(forward, 0.3f));
                shootPosition = Vector3Add(shootPosition, Vector3Scale(up, -0.5f));
                if (shootCanonIndex == 0)
                    shootPosition = Vector3Add(shootPosition, Vector3Scale(right, 1.55f));
                else
                    shootPosition = Vector3Add(shootPosition, Vector3Scale(right, -1.55f));

                BulletManager::GetInstance()->CreateBullet(shootPosition, GetForward(),STAT_BULLET_DAMAGE_FIGHTER, 0, myTeam);
                ParticleManager::GetInstance()->CreateShootMuzzle(shootPosition, this);
            }
        }
    }
}
