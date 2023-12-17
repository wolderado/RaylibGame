//
// Created by Salih on 12-Dec-23.
//

#include "GameObject.h"

void GameObject::Update(float deltaTime) {

    Position = Vector3Add(Position,currentVelocity);
}


void ConvertMeshToWires(Mesh targetMesh,Vector3 centerPos, float radius, Color color)
{

}

void GameObject::Render(float deltaTime) {

    if(myRenderType == RealMesh)
    {
        Renderer::GetInstance()->RenderModelWithWires(myModel,Position,Rotation, Vector3Scale(Scale,1.01f),myColor);

        if(DEBUG_SHOW_COLLISION_AREA) {
            if (CanCollide) {
                DrawSphereWires(Position, CollisionSize, 16, 16, PURPLE);
            }
        }
    }


}

void GameObject::Hurt(float damage) {

}

GameObject::GameObject() {
    Position = {0, 0, 10};
    Rotation = Vector3Zero();
    Scale = Vector3One();

    SetTeam(TEAM_NEUTRAL);
}

void GameObject::SetModel(Model model) {

/*    if(myRenderType == RealMesh)
        UnloadModel(myModel);*/

    myModel = model;
    myRenderType = RealMesh;
}

void GameObject::Destroy() {

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

    Vector3 awayDir = Vector3Subtract(Position,otherObject->Position);
    float totalVel = (Vector3Length(collisionTotalVelocity)) / Mass;
    const float bounciness =  0.5f;
    currentVelocity = Vector3Scale(Vector3Normalize(awayDir),totalVel * bounciness);

}