//
// Created by Salih on 10-Dec-23.
//

#include "Renderer.h"





void Renderer::InitRenderer(Player* pla) {

    player = shared_ptr<Player>(pla);
    camera = shared_ptr<Camera>(pla->GetCamera());

    backgroundTexture = LoadTexture("resources/skybox.png");

    for (int i = 0; i < sizeof(starPositions) / sizeof(starPositions[0]); ++i) {
        starPositions[i].x = GetRandomValue(-100,100);
        starPositions[i].y = GetRandomValue(-100,100);
        starPositions[i].z = GetRandomValue(-100,100);

        starPositions[i] = Vector3Normalize(starPositions[i]);
    }

}


float CalculateDistanceToDotCenter(float x,float y,float z, float maxDistance)
{
    float xRatio = fabs( x ) / maxDistance;
    float yRatio = fabs(y) / maxDistance;
    float zRatio = fabs(z) / maxDistance;

    float ratio = (xRatio + yRatio + zRatio) / 3.0f;
    return ratio;
}




void Renderer::RenderBackground() {
    ClearBackground(PALETTE_GRAY1);
    //DrawTexturePro(backgroundTexture, (Rectangle){ 0, 0, (float)backgroundTexture.width, -(float)backgroundTexture.height }, (Rectangle){ 0, 0, (float)RenderWidth, (float)RenderHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);


/*    for (Vector3 star : starPositions){
        Vector3 pos = Vector3Scale(star, 10000);
        Vector2 screenPos = GetWorldToScreenEx(pos, *camera, RenderWidth, RenderHeight);
        DrawPixel(screenPos.x, screenPos.y, RED);
        //DrawSphere(pos, 0.1f, PALETTE_GRAY2);
    }*/


    DrawText(TextFormat("FPS: %3.1f", (1.0f/GetFrameTime())), 10, 10, 10, PALETTE_GRAY2);
}



void Renderer::RenderAtmosphere() {

    DrawDots();
}

bool Renderer::IsVisible(Vector3 position) {

    Vector3 toTarget = Vector3Subtract(position,player->Position);
    return Vector3DotProduct(Vector3Normalize(toTarget), Vector3Normalize(GetCameraForward(camera.get()))) > 0.0f;
}

void Renderer::Unload() {
    UnloadTexture(backgroundTexture);
}

void Renderer::DrawDots() {
    //O(n^3) complexity
    float scale = 1.0f;
    float dotDistance = 6.0f;
    float lineLength = player->GetVelocityRatioToMaxValue() * maxLineLength;
    lineLength = fmax(lineLength, 0.02f);
    Vector3 playerVelocity = player->GetVelocityNormalized();

    for (float x = -dotDistance; x < dotDistance; x +=scale) {
        for (float y = -dotDistance; y < dotDistance; y +=scale) {
            for (float z = -dotDistance; z < dotDistance; z +=scale) {

                Vector3 modPlayerPos;
                modPlayerPos.x = round(player->Position.x / scale) * scale;
                modPlayerPos.y = round(player->Position.y / scale) * scale;
                modPlayerPos.z = round(player->Position.z / scale) * scale;

                Vector3 pos = Vector3Add(modPlayerPos,(Vector3) {x, y, z});

                Vector3 velocityDir = Vector3Scale(playerVelocity, lineLength);
                Vector3 lineEndPos = Vector3Add(pos,velocityDir);


                //Cull dots
                if(!IsVisible(pos) && !IsVisible(lineEndPos) )
                    continue;


                float distanceSquared = Vector3LengthSqr(Vector3Subtract(pos, player->Position));
                float ratio = 1.0f - (distanceSquared / (dotDistance * dotDistance ));
                ratio = ratio * ratio;
                ratio = Clamp(ratio, 0.0f, 1.0f);

                Color dotColor = PALETTE_GRAY2;
                dotColor = ColorAlpha(dotColor,ratio);

                DrawLine3D(pos, lineEndPos, dotColor);
            }
        }
    }
}
