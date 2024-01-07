//
// Created by Salih on 12-Dec-23.
//

#include "GameObject.h"

void GameObject::Update(float deltaTime) {

    if (!enabled)
        return;
/*
    if(myTeam == TEAM_ENEMY)
        DrawLine3D(Position,Vector3Zero(),RED);*/


    Vector3 frameVelocity = Vector3Scale(currentVelocity, deltaTime * 100);
    Position = Vector3Add(Position, frameVelocity);
/*
    if (myTeam == TEAM_NEUTRAL) {
        if (Vector3Length(currentVelocity) > 0)
            cout << Vector3Length(currentVelocity) << endl;
    }*/
}


void ConvertMeshToWires(Mesh targetMesh,Vector3 centerPos, float radius, Color color)
{

}

void GameObject::Render(float deltaTime) {



    if(!enabled) {
        cout << "DISABLED" << endl;
        return;
    }


    Color modelColor = myColor;
    Vector3 modelScale = Scale;

    if(GetTime() - LastHurtTime < FlashDuration) {
        float t = (GetTime() - LastHurtTime) / FlashDuration;
        t = pow(t,1.5f);
        modelColor = Utility::LerpColor(hurtFlashColor, myColor, t);
        modelScale = Vector3Lerp((Vector3){Scale.x * 0.9f,Scale.y * 0.9f,Scale.z * 0.9f},modelScale, t);
    }

    if(myRenderType == RealMesh)
    {
        Renderer::GetInstance()->RenderModelWithWires(myModel,Position, QuaternionToEuler(Rotation), Vector3Scale(modelScale,1.01f),modelColor);
    }


    if(DEBUG_SHOW_COLLISION_AREA) {
        if (CanCollide && IgnoresAllCollisions == false) {
            DrawSphereWires(Position, CollisionSize, 16, 16, PURPLE);
        }
    }
}

void GameObject::Hurt(float damage) {

    if(!enabled)
        return;

    health -= damage;
    LastHurtTime = GetTime();
    if(health <= 0)
    {
        health = 0;
    }
}

GameObject::GameObject() {


    Position = {0, 0, 10};
    Rotation = QuaternionIdentity();
    Scale = Vector3One();
    currentVelocity = Vector3Zero();
    CanCollide = true;
    IgnoresAllCollisions = false;

    health = 1;
    maxHealth = health;
    LastHurtTime = -999;
    enabled = true;
    GridIndex = make_tuple(0,0,0);
    tags = unordered_set<string>();

    SetTeam(TEAM_NEUTRAL);
}

void GameObject::SetModel(Model model) {

/*    if(myRenderType == RealMesh)
        UnloadModel(myModel);*/

    myModel = model;
    myRenderType = RealMesh;
}

void GameObject::Destroy() {

    if(!enabled)
        return;

    enabled = false;
    health = 0;

/*    if(myRenderType == RealMesh)
        UnloadModel(myModel);*/
}

TEAM GameObject::GetTeam() {
    return myTeam;
}

void GameObject::SetTeam(TEAM newTeam) {
    myTeam = newTeam;

    if(myTeam == TEAM_NEUTRAL)
        myColor = PALETTE_GRAY4;
    else if(myTeam == TEAM_PLAYER)
        myColor = PALETTE_BLUE2;
    else if(myTeam == TEAM_ENEMY)
        myColor = PALETTE_RED2;
    else if(myTeam == TEAM_ALLY)
        myColor = PALETTE_GREEN2;
}

void GameObject::OnCollision(GameObject *otherObject,Vector3 collisionTotalVelocity) {
/*
    cout << Name << " collided with " << otherObject->Name << endl;*/

    if(!enabled || GetTime()-lastCollisionTime < 0.5f )
        return;

    lastCollisionTime = GetTime();

    Vector3 awayDir = Vector3Subtract(Position,otherObject->Position);
    float totalVel = (Vector3Length(collisionTotalVelocity)) / Mass;
    const float bounciness =  1.0f;
    totalVel = fmin(totalVel, 0.1f);
    currentVelocity = Vector3Scale(Vector3Normalize(awayDir),totalVel * bounciness);

}

void GameObject::LateUpdate(float deltaTime) {

    if(!enabled)
        return;
}

float GameObject::GetHealth() {
    return health;
}

void GameObject::SetVelocity(Vector3 newVelocity) {

    currentVelocity = newVelocity;
}

void GameObject::OnInit() {

    if(myTeam == TEAM_ENEMY)
        hurtFlashColor = PALETTE_YELLOW1;
}


//TODO: Implement a more accurate calculation for calculating forward, right and up vectors
Vector3 GameObject::GetForward() {
    Matrix mat = QuaternionToMatrix(Rotation);
    Vector3 forward = { mat.m8, mat.m9, mat.m10 };

    if(forward.x == NAN || forward.y == NAN || forward.z == NAN)
        return VECTOR3_FORWARD;

    return forward;
}

Vector3 GameObject::GetRight() {
/*    Vector3 right = Vector3CrossProduct(VECTOR3_UP, GetForward());
    return right;*/

    Matrix mat = QuaternionToMatrix(Rotation);
    Vector3 right = { mat.m0, mat.m1, mat.m2 };

    if(right.x == NAN || right.y == NAN || right.z == NAN)
        return VECTOR3_RIGHT;

    return right;
}

Vector3 GameObject::GetUp() {
/*    Vector3 up = Vector3CrossProduct(GetForward() , GetRight());
    return up;*/


    Matrix mat = QuaternionToMatrix(Rotation);
    Vector3 up = { mat.m4, mat.m5, mat.m6 };

    if(up.x == NAN || up.y == NAN || up.z == NAN)
        return VECTOR3_UP;

    return up;
}

void GameObject::SetHealth(float newMaxHealth) {
    maxHealth = newMaxHealth;
    health = maxHealth;
}

bool GameObject::IsEnabled() {
    return enabled;
}

void GameObject::Heal(float amount) {

    health += amount;
    if(health > maxHealth)
        health = maxHealth;
}
