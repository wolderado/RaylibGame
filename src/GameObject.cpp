//
// Created by Salih on 12-Dec-23.
//

#include "GameObject.h"

void GameObject::Update(float deltaTime) {

}

void GameObject::Render(float deltaTime) {


    if(myRenderType == RealMesh)
    {
        myModel.transform = MatrixRotateXYZ(QuaternionToEuler(Transform.rotation));
        myModel.transform = MatrixScale(Transform.scale.x, Transform.scale.y, Transform.scale.z);

        DrawModel(myModel, Transform.translation, 1.0f, FAKE_TRANSPARENT1);
        DrawModelWires(myModel, Transform.translation, 1.0f, myColor);
    }

    //DrawCubeWires(Transform.translation, Transform.scale.x, Transform.scale.y, Transform.scale.z, PALETTE_RED2);
    //DrawCube(Transform.translation, Transform.scale.x, Transform.scale.y, Transform.scale.z, FAKE_TRANSPARENT1);
}

void GameObject::Hurt(float damage) {

}

GameObject::GameObject() {
    Transform.translation = {0, 0, 10};
    Transform.rotation = QuaternionIdentity();
    Transform.scale = Vector3One();

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
