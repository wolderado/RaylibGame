//
// Created by Salih on 12-Dec-23.
//

#include "GameObject.h"

void GameObject::Update(float deltaTime) {

}


void ConvertMeshToWires(Mesh targetMesh,Vector3 centerPos, float radius, Color color)
{

}

void GameObject::Render(float deltaTime) {


    if(myRenderType == RealMesh)
    {
        //Matrix rotMatrix = MatrixRotateXYZ(Rotation);
        //Matrix scaleMatrix = MatrixScale(Scale.x, Scale.y, Scale.z);


        //myModel.transform = MatrixMultiply(rotMatrix, scaleMatrix);
        //myModel.transform = MatrixScale(Scale.x, Scale.y, Scale.z);

        //DrawModel(myModel, Position, 1.0f, FAKE_TRANSPARENT1);
        /*glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            DrawModelWiresEx(myModel, Position, (Vector3){0,1,0},0, Vector3One(), FAKE_TRANSPARENT1);*/

        Renderer::GetInstance()->RenderModelWithWires(myModel,Position,Rotation, Vector3Scale(Scale,1.01f),myColor);

/*        Renderer::GetInstance()->RenderModel(myModel, Position,Rotation, Scale, FAKE_TRANSPARENT1);
        Renderer::GetInstance()->RenderModelWire(myModel,Position,Rotation, Vector3Scale(Scale,1.01f),myColor);*/
    }

    //DrawCubeWires(Transform.translation, Transform.scale.x, Transform.scale.y, Transform.scale.z, PALETTE_RED2);
    //DrawCube(Transform.translation, Transform.scale.x, Transform.scale.y, Transform.scale.z, FAKE_TRANSPARENT1);
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

