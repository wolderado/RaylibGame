//
// Created by Salih on 16-Dec-23.
//

#include "HUD.h"

void HUD::Init(shared_ptr<Player> targetPlayer)
{
    //cockpitTexture = LoadTexture("resources/Cockpit.png");

    playerGunModel = LoadModel("resources/PlayerGun.glb");
    player = shared_ptr<Player>(targetPlayer);
}


void HUD::Render(float deltaTime)
{
/*    DrawTexturePro(cockpitTexture, (Rectangle){0,0,(float)cockpitTexture.width,(float)cockpitTexture.height},
                   (Rectangle){0,0,ScreenWidth,ScreenHeight}, (Vector2){0,0}, 0, WHITE);*/
}



void HUD::Render3D(float deltaTime)
{
    DrawGuns(deltaTime);


    if(DEBUG_SHOW_GIZMO) {
        //forward = Vector3Scale(forward,10.0f);
        DrawLine3D(Vector3Zero(), (Vector3) {0, 0, 10}, BLUE);
        DrawLine3D(Vector3Zero(), (Vector3) {0, 10, 0}, GREEN);
        DrawLine3D(Vector3Zero(), (Vector3) {10, 0, 0}, RED);
    }
}


void HUD::Unload()
{
    //UnloadTexture(cockpitTexture);
}

void HUD::DrawGuns(float deltaTime) {

    Vector3 sway = player->GetSwayInput();
    smoothSway = Vector3Lerp(smoothSway,sway,swaySpeed * deltaTime);

    Vector3 angleSway = Vector3Scale((Vector3){-smoothSway.x,smoothSway.y,smoothSway.y},swayAngleAmount);
    Vector3 positionSway = Vector3Scale((Vector3){-smoothSway.y,-smoothSway.x,-smoothSway.z},swayPositionAmount);

    //Gun Renderings

    //Left Gun
    Vector3 weaponLeftPos = (Vector3){5.5f,-1.0f,5.0f};
    weaponLeftPos = Vector3Add(weaponLeftPos,positionSway);
    Vector3 gunRotationLeft = Vector3Scale((Vector3){0,180,0},DEG2RAD);
    angleSway.z = -angleSway.z;
    gunRotationLeft = Vector3Add(gunRotationLeft,angleSway);
    Renderer::GetInstance()->RenderModelWithWires(playerGunModel,weaponLeftPos, gunRotationLeft,Vector3One(),PALETTE_BLUE2, true);


    //Right Gun
    Vector3 weaponRightPos = (Vector3){-5.5f,-1.0f,5.0f};
    weaponRightPos = Vector3Add(weaponRightPos,positionSway);
    Vector3 gunRotationRight = Vector3Scale((Vector3){0,180,0},DEG2RAD);
    gunRotationRight = Vector3Add(gunRotationRight,(Vector3){angleSway.x,-angleSway.y,angleSway.z});
    Vector3 mirroredScale = (Vector3){-1,1,1};
    Renderer::GetInstance()->RenderModelWithWires(playerGunModel,weaponRightPos, gunRotationRight,mirroredScale,PALETTE_BLUE2, true);

}