//
// Created by Salih on 02-Jan-24.
//

#include "Fighter.h"


void Fighter::Update(float deltaTime) {

/*    rotateTimer+= deltaTime;
    if(rotateTimer> 5)
    {
        rotateTimer = 0;
        currentVelocity = Vector3Scale(Utility::GetRandomDirection(),0.001f);
    }*/

/*    Quaternion lookRot = Utility::LookAt(Vector3Zero(), currentVelocity);


    smoothRot = QuaternionLerp(smoothRot, lookRot, deltaTime * 1.0f);

    Rotation = QuaternionToEuler(smoothRot);*/


/*    Rotation.y += 10 * deltaTime * DEG2RAD;
    Rotation.x = 0;
    Rotation.z = 0;*/


    //ParticleManager::GetInstance()->CreateShootMuzzle((Vector3){0,1.0f,0}, this);


    selectTargetTimer += deltaTime;
    if(selectTargetTimer > 5)
    {
        selectTargetTimer = 0;
        FindANewTarget();
    }

    GameObject::Update(deltaTime);


    if(CurrentAIState == AttackTarget) {
        if (target == nullptr || target->GetHealth() <= 0) {
            FindANewTarget();
            return;
        }


        Vector3 toTargetDir = Vector3Normalize(Vector3Subtract(target->Position, Position));
        //DrawLine3D(Position, target->Position, RED);

        lookRot = Utility::LookAt(Position,target->Position);

        Vector3 thrust = Vector3Scale(GetForward(), ThrustSpeed * deltaTime);
        currentVelocity = Vector3Add(currentVelocity, thrust);


    }



    smoothRot = QuaternionLerp(smoothRot, lookRot, deltaTime * RotateSpeed);
    Rotation = smoothRot;


    currentVelocity = Vector3ClampValue(currentVelocity, -MaxVelocity, MaxVelocity);

    //DrawLine3D(Position, Vector3Add(Position,Vector3Scale(forward,10)), PURPLE);
}


void Fighter::Destroy() {
    GameObject::Destroy();

    ParticleManager::GetInstance()->CreateAsteroidExplosion(Position,10);
}

Fighter::Fighter(TEAM team, Vector3 position) : GameObject() {

    myTeam = team;
    Name = "Fighter";
    Position = position;
    FindANewTarget();
    selectTargetTimer = GetRandomValue(0,500) * 0.01f;

}

void Fighter::Render(float deltaTime) {
    GameObject::Render(deltaTime);


/*    DrawLine3D(Position, Vector3Add(Position, (Vector3){10,0,0}), RED);
    DrawLine3D(Position, Vector3Add(Position, (Vector3){0,10,0}), YELLOW);
    DrawLine3D(Position, Vector3Add(Position, (Vector3){0,0,10}), BLUE);

    DrawLine3D(Position, Vector3Add(Position, Vector3Scale(Vector3Normalize(currentVelocity), 10)), GREEN);*/
}

void Fighter::OnInit() {
    GameObject::OnInit();

    currentVelocity = Vector3Scale(Utility::GetRandomDirection(),0.001f);
}

void Fighter::FindANewTarget() {
    CurrentAIState = FindTarget;
}

void Fighter::SetTarget(shared_ptr<GameObject> newTarget) {
    CurrentAIState = AttackTarget;
    target = shared_ptr<GameObject>(newTarget);
}
