//
// Created by Salih on 12-Dec-23.
//

#include "GameObject.h"

void GameObject::Update(float deltaTime) {

    if(!enabled)
        return;

    Position = Vector3Add(Position,currentVelocity);
}


void ConvertMeshToWires(Mesh targetMesh,Vector3 centerPos, float radius, Color color)
{

}

void GameObject::Render(float deltaTime) {

    if(!enabled)
        return;

    Color modelColor = myColor;
    Vector3 modelScale = Scale;

    if(GetTime() - lastHurtTime < flashDuration) {
        float t = (GetTime() - lastHurtTime) / flashDuration;
        t = pow(t,1.5f);
        modelColor = Utility::LerpColor(PALETTE_RED1, myColor, t);
        modelScale = Vector3Lerp((Vector3){Scale.x * 0.9f,Scale.y * 0.9f,Scale.z * 0.9f},modelScale, t);
    }

    if(myRenderType == RealMesh)
    {
        Renderer::GetInstance()->RenderModelWithWires(myModel,Position,Rotation, Vector3Scale(modelScale,1.01f),modelColor);

        if(DEBUG_SHOW_COLLISION_AREA) {
            if (CanCollide) {
                DrawSphereWires(Position, CollisionSize, 16, 16, PURPLE);
            }
        }
    }

    if(ShowHealthBar)
    {

        Renderer::GetInstance()->RenderHealthBar(Position,Scale,health,maxHealth);
    }
}

void GameObject::Hurt(float damage) {

    if(!enabled)
        return;

    health -= damage;
    lastHurtTime = GetTime();
    if(health <= 0)
    {
        health = 0;
    }
}

GameObject::GameObject() {
    Position = {0, 0, 10};
    Rotation = Vector3Zero();
    Scale = Vector3One();
    currentVelocity = Vector3Zero();
    health = 1;
    maxHealth = health;
    lastHurtTime = -999;

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

    if(myRenderType == RealMesh)
        UnloadModel(myModel);
}

TEAM GameObject::GetTeam() {
    return myTeam;
}

void GameObject::SetTeam(TEAM newTeam) {
    myTeam = newTeam;

    if(myTeam == TEAM_NEUTRAL)
        myColor = PALETTE_GRAY2;
    else if(myTeam == TEAM_PLAYER)
        myColor = PALETTE_BLUE2;
    else if(myTeam == TEAM_ENEMY)
        myColor = PALETTE_RED2;
}

void GameObject::OnCollision(GameObject *otherObject,Vector3 collisionTotalVelocity) {

    if(!enabled)
        return;

    Vector3 awayDir = Vector3Subtract(Position,otherObject->Position);
    float totalVel = (Vector3Length(collisionTotalVelocity)) / Mass;
    const float bounciness =  0.5f;
    currentVelocity = Vector3Scale(Vector3Normalize(awayDir),totalVel * bounciness);

}

void GameObject::LateUpdate(float deltaTime) {

    if(!enabled)
        return;
}

float GameObject::GetHealth() {
    return health;
}
