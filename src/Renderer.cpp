//
// Created by Salih on 10-Dec-23.
//

#include "Renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer() {
    instance = this;
}


Renderer* Renderer::GetInstance() {
    if(instance == nullptr)
        instance = new Renderer();

    return instance;
}


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

    string modelPath = "resources/AsteroidLOD.glb";
    GenericLODModel = LoadModel(modelPath.c_str());

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


   // DrawText(TextFormat("FPS: %3.1f", (1.0f/GetFrameTime())), 10, 10, 10, PALETTE_GRAY2);
}



void Renderer::RenderAtmosphere() {

    DrawDots();
}



void Renderer::Unload() {
    UnloadTexture(backgroundTexture);
    UnloadModel(GenericLODModel);
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

void Renderer::RenderModelWire(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color) {

    //Trace vertices with correct indices

    //Culling
    if(IsVisible(position,ModelsCullDotValue) == false)
        return;

    float distToPlayer = Vector3DistanceSqr(position,player->Position);
    if(distToPlayer > LOD2Distance * LOD2Distance)
        return;

    Mesh targetMesh = targetModel.meshes[0];

    if(distToPlayer > LOD1Distance * LOD1Distance)
        targetMesh = GenericLODModel.meshes[0];


    rlPushMatrix();

    //Transforms
    Matrix finalMatrix = MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), MatrixRotateXYZ(rotation));
    finalMatrix = MatrixMultiply(finalMatrix, MatrixTranslate(position.x, position.y, position.z));
    rlMultMatrixf(MatrixToFloat(finalMatrix));

    //Line rendering
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    for (int i = 0; i < targetMesh.triangleCount * 3; i += 3)
    {
        unsigned short index1 = targetMesh.indices[i];
        unsigned short index2 = targetMesh.indices[i + 1];
        unsigned short index3 = targetMesh.indices[i + 2];

        // Draw lines for each edge of the triangle
        rlVertex3f(targetMesh.vertices[3 * index1], targetMesh.vertices[3 * index1 + 1], targetMesh.vertices[3 * index1 + 2]);
        rlVertex3f(targetMesh.vertices[3 * index2], targetMesh.vertices[3 * index2 + 1], targetMesh.vertices[3 * index2 + 2]);

        rlVertex3f(targetMesh.vertices[3 * index2], targetMesh.vertices[3 * index2 + 1], targetMesh.vertices[3 * index2 + 2]);
        rlVertex3f(targetMesh.vertices[3 * index3], targetMesh.vertices[3 * index3 + 1], targetMesh.vertices[3 * index3 + 2]);

        rlVertex3f(targetMesh.vertices[3 * index3], targetMesh.vertices[3 * index3 + 1], targetMesh.vertices[3 * index3 + 2]);
        rlVertex3f(targetMesh.vertices[3 * index1], targetMesh.vertices[3 * index1 + 1], targetMesh.vertices[3 * index1 + 2]);
    }

    rlEnd();
    rlPopMatrix();
}

void Renderer::RenderModel(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color) {

    if(IsVisible(position,ModelsCullDotValue) == false)
        return;

    //Don't render inside of it's too far. Only render wires
    if(Vector3DistanceSqr(position,player->Position) > LOD1Distance * LOD1Distance)
        return;


    //Transforms
    Matrix rotMatrix = MatrixRotateXYZ(rotation);
    Matrix scaleMatrix = MatrixScale(scale.x, scale.y, scale.z);
    targetModel.transform = MatrixMultiply(rotMatrix, scaleMatrix);

    //Draw
    DrawModel(targetModel, position, 1.0f, color);

}



Color LerpColor(Color a, Color b, float t)
{
    Color result;
    result.r = (unsigned char) (a.r + (b.r - a.r) * t);
    result.g = (unsigned char) (a.g + (b.g - a.g) * t);
    result.b = (unsigned char) (a.b + (b.b - a.b) * t);
    result.a = (unsigned char) (a.a + (b.a - a.a) * t);
    return result;
}



//Draws both inside and wireframe of the object. Also fades in the LODs
void Renderer::RenderModelWithWires(Model targetModel, Vector3 position, Vector3 rotation, Vector3 scale, Color color) {

    RenderModel(targetModel, position,rotation, scale, FAKE_TRANSPARENT1);
    RenderModelWire(targetModel,position,rotation, Vector3Scale(scale,1.01f),color);
}


bool Renderer::IsVisible(Vector3 position) {

    Vector3 toTarget = Vector3Subtract(position,player->Position);
    return Vector3DotProduct(Vector3Normalize(toTarget), Vector3Normalize(GetCameraForward(camera.get()))) > 0.0f;
}


bool Renderer::IsVisible(Vector3 position, float customDotValue) {
    Vector3 toTarget = Vector3Subtract(position,player->Position);
    return Vector3DotProduct(Vector3Normalize(toTarget), Vector3Normalize(GetCameraForward(camera.get()))) > customDotValue;
}
