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


void Renderer::InitRenderer(Camera* cam) {

    //player = shared_ptr<Player>(pla);
    camera = shared_ptr<Camera>(cam);

    string modelPath = "resources/AsteroidLOD.glb";
    genericLODModel = LoadModel(modelPath.c_str());

    //Skybox generation
    skyboxModel = LoadModel("resources/skybox.glb");
    skyboxTexture = LoadTexture("resources/skyboxSeamless.png");
    skyboxModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyboxTexture;


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
    //Draw skybox
    //DrawModel(skyboxModel, camera->position, 100.0f, WHITE);
/*
    float scale = 64;
    Vector3 dirVector = Vector3Normalize(GetCameraForward(camera.get()));
*//*    DrawTexturePro(skyboxTexture, (Rectangle){ dirVector.x * skyboxTexture.width, dirVector.y * skyboxTexture.width, scale, scale },
                   (Rectangle){ 0, 0, RenderWidth, RenderHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);*//*


   // DrawText(TextFormat("FPS: %3.1f", (1.0f/GetFrameTime())), 10, 10, 10, PALETTE_GRAY2);*/



/*    float scale = 0.25f;
    Vector3 forward = Vector3Add(camera->position,(Vector3){0,0,1});
    Vector2 screenPoint = GetWorldToScreen(forward,*camera);

    DrawTexturePro(skyboxTexture, (Rectangle){-screenPoint.x * scale, -screenPoint.y * scale, (float)skyboxTexture.width, (float)skyboxTexture.height },
                   (Rectangle){ 0, 0, RenderWidth, RenderHeight }, (Vector2){ 0.5f,  0.5f }, 0.0f, WHITE);*/
}



void Renderer::RenderAtmosphere(float cameraVelocityRatio,Vector3 cameraVelocity) {

    //DrawModel(skyboxModel, camera->position, 100.0f, WHITE);

    DrawDots(cameraVelocityRatio,cameraVelocity);

}



void Renderer::Unload() {
    UnloadTexture(skyboxTexture);
    UnloadModel(genericLODModel);
    UnloadModel(skyboxModel);
}

void Renderer::DrawDots(float cameraVelocityRatio,Vector3 cameraVelocity) {
    //O(n^3) complexity
    float scale = 1.5f;
    float dotDistance = 6.0f;
    float lineLength = cameraVelocityRatio * maxLineLength;
    lineLength = fmax(lineLength, 0.02f);
    Vector3 playerVelocity = Vector3Normalize(cameraVelocity);

    for (float x = -dotDistance; x < dotDistance; x +=scale) {
        for (float y = -dotDistance; y < dotDistance; y +=scale) {
            for (float z = -dotDistance; z < dotDistance; z +=scale) {

                Vector3 modPlayerPos;
                modPlayerPos.x = round(camera->position.x / scale) * scale;
                modPlayerPos.y = round(camera->position.y / scale) * scale;
                modPlayerPos.z = round(camera->position.z / scale) * scale;

                Vector3 pos = Vector3Add(modPlayerPos,(Vector3) {x, y, z});

                Vector3 velocityDir = Vector3Scale(playerVelocity, lineLength);
                Vector3 lineEndPos = Vector3Add(pos,velocityDir);


                //Cull dots
                if(!IsVisible(pos) && !IsVisible(lineEndPos) )
                    continue;


                float distanceSquared = Vector3LengthSqr(Vector3Subtract(pos, camera->position));
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


    float distToPlayer = Vector3DistanceSqr(position,camera->position);
    Mesh targetMesh = targetModel.meshes[0];



    //Transforms
    Matrix rotationMatrix = MatrixRotateXYZ((Vector3){ rotation.x, rotation.y, rotation.z });
    Matrix scaleMatrix = MatrixScale(scale.x, scale.y, scale.z);
    Matrix translationMatrix = MatrixTranslate(position.x, position.y, position.z);
    Matrix transformationMatrix = MatrixMultiply(MatrixMultiply(rotationMatrix, scaleMatrix), translationMatrix);

    rlPushMatrix();
    rlMultMatrixf(MatrixToFloat(transformationMatrix));
    rlEnableBackfaceCulling();

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

    rlDisableBackfaceCulling();
    rlEnd();
    rlPopMatrix();
}

void Renderer::RenderModel(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color) {


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
void Renderer::RenderModelWithWires(Model targetModel, Vector3 position, Vector3 rotation, Vector3 scale, Color color,bool ignoreOptimizations) {

    if(targetModel.meshCount > 1)
    {
        for (int i = 0; i < targetModel.meshCount; ++i) {
            RenderModelWithWires(LoadModelFromMesh(targetModel.meshes[i]),position,rotation,scale,color,ignoreOptimizations);
        }

        return;
    }


    float distToPlayer = Vector3DistanceSqr(position, camera->position);
    //Scale lines based on distance to prevent clashing with fill model
    float distRatioLOD1 = distToPlayer / (LOD1Distance * LOD1Distance);
    float offsetScale = Lerp(1.01f,1.4f,distRatioLOD1);

    if(ignoreOptimizations == false) {

        //Frustum culling
        if (IsVisible(position, ModelsCullDotValue) == false)
            return;

        //TODO: Convert this from real distance to screen size
        //Distance culling
        if (distToPlayer > CullDistance * CullDistance)
            return;


        if(distToPlayer > LOD1Distance * LOD1Distance){
            //Generic LOD mesh
            targetModel = LoadModelFromMesh(genericLODModel.meshes[0]);
            //Scale up to hide lod change
            scale = Vector3Scale(scale,1.5f);

            //Smooth scaling to prevent LOD popping
            float distRatio = (distToPlayer  - (LODScaleDistanceOffset * LODScaleDistanceOffset)) / (CullDistance * CullDistance);
            distRatio = fmin(distRatio,1.0f);
            distRatio = fmax(distRatio,0.0f);
            scale = Vector3Scale(scale,1.0f - distRatio );
        }

        //Ignore fill model if too far
        if (distToPlayer < LOD1Distance * LOD1Distance)
            RenderModel(targetModel, position, rotation, scale, FAKE_TRANSPARENT1);


    }else
    {
        RenderModel(targetModel, position, rotation, scale, FAKE_TRANSPARENT1);
        offsetScale = 1.01f;
    }


    RenderModelWire(targetModel,position,rotation, Vector3Scale(scale,offsetScale),color);
}



bool Renderer::IsVisible(Vector3 position) {

    Vector3 toTarget = Vector3Subtract(position,camera->position);
    return Vector3DotProduct(Vector3Normalize(toTarget), Vector3Normalize(GetCameraForward(camera.get()))) > 0.0f;
}


bool Renderer::IsVisible(Vector3 position, float customDotValue) {
    Vector3 toTarget = Vector3Subtract(position,camera->position);
    return Vector3DotProduct(Vector3Normalize(toTarget), Vector3Normalize(GetCameraForward(camera.get()))) > customDotValue;
}