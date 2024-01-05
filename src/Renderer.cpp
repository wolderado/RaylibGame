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
    //healthBarsTextureSheet = LoadTexture("resources/healthBars.png");
    billboardTextureSheet = LoadTexture("resources/BillboardSheet.png");
    skyboxModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyboxTexture;
    alphaCutoffShader = LoadShader(NULL, "resources/AlphaCutoff.fs");


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


    //Draw skybox
    //DrawModel(skyboxModel, camera->position, 100.0f, WHITE);
/*
    float scale = 64;
    Vector3 dirVector = Vector3Normalize(GetCameraForward(camera.get()));
*//*    DrawTexturePro(skyboxTexture, (Rectangle){ dirVector.x * skyboxTexture.width, dirVector.y * skyboxTexture.width, scale, scale },
                   (Rectangle){ 0, 0, RenderWidth, RenderHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);*//*


   // DrawText(TextFormat("FPS: %3.1f", (1.0f/GetFrameTime())), 10, 10, 10, PALETTE_GRAY4);*/



/*    float scale = 0.25f;
    Vector3 forward = Vector3Add(camera->position,(Vector3){0,0,1});
    Vector2 screenPoint = GetWorldToScreen(forward,*camera);

    DrawTexturePro(skyboxTexture, (Rectangle){-screenPoint.x * scale, -screenPoint.y * scale, (float)skyboxTexture.width, (float)skyboxTexture.height },
                   (Rectangle){ 0, 0, RenderWidth, RenderHeight }, (Vector2){ 0.5f,  0.5f }, 0.0f, WHITE);*/
}



void Renderer::RenderAtmosphere(float cameraVelocityRatio,Vector3 cameraVelocity) {

    DrawDots(cameraVelocityRatio,cameraVelocity);



}



void Renderer::Unload() {
    UnloadTexture(skyboxTexture);
    UnloadTexture(billboardTextureSheet);
    //UnloadTexture(healthBarsTextureSheet);
    UnloadModel(genericLODModel);
    UnloadModel(skyboxModel);
}

void Renderer::DrawDots(float cameraVelocityRatio,Vector3 cameraVelocity) {
    //O(n^3) complexity
    float scale = 3.0f;
    float dotDistance = 12.0f;
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

                Color dotColor = Utility::LerpColor(PALETTE_GRAY3, SKY_COLOR, ratio);
                //dotColor.a = (unsigned char) (ratio * 255.0f);

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
        //If model has multiple meshes, render them recursively
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

void Renderer::RenderBillboard(int spriteRowIndex,int spriteIndex, Vector3 position, float size,float rotation,Color simplifiedColor) {

    Vector3 cameraUp = GetCameraUp(camera.get());
    DrawBillboard(*camera, billboardTextureSheet,
                  (Rectangle) {(float) spriteIndex * 16, (float) spriteRowIndex * 16, 16, 16}, position, cameraUp,
                  (Vector2) {size, size}, (Vector2) {0, 0}, rotation, WHITE);

/*    float distSq = Vector3DistanceSqr(position, camera->position);

    if (distSq < 100 * 100) {
        Vector3 cameraUp = GetCameraUp(camera.get());
        DrawBillboard(*camera, billboardTextureSheet,
                      (Rectangle) {(float) spriteIndex * 16, (float) spriteRowIndex * 16, 16, 16}, position, cameraUp,
                      (Vector2) {size, size}, (Vector2) {0, 0}, rotation, WHITE);
    } else {
        Vector2 screenPos = GetWorldToScreenEx(position, *camera,RenderWidth,RenderHeight);
        DrawPixelV(screenPos, simplifiedColor);
    }*/
}

void Renderer::RenderHealthBar(Vector3 position,Vector3 scale,float currentHealth,float maxHealth) {

}


//Modified function from rlgl.c DrawBillboardPro
void Renderer::DrawBillboard(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector3 up, Vector2 size,
                        Vector2 origin, float rotation, Color tint) {


    // NOTE: Billboard size will maintain source rectangle aspect ratio, size will represent billboard width
    Vector2 sizeRatio = { size.x*fabsf((float)source.width/source.height), size.y };

    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    Vector3 right = { matView.m0, matView.m4, matView.m8 };
    //Vector3 up = { matView.m1, matView.m5, matView.m9 };

    Vector3 rightScaled = Vector3Scale(right, sizeRatio.x/2);
    Vector3 upScaled = Vector3Scale(up, sizeRatio.y/2);

    Vector3 p1 = Vector3Add(rightScaled, upScaled);
    Vector3 p2 = Vector3Subtract(rightScaled, upScaled);

    Vector3 topLeft = Vector3Scale(p2, -1);
    Vector3 topRight = p1;
    Vector3 bottomRight = p2;
    Vector3 bottomLeft = Vector3Scale(p1, -1);

    if (rotation != 0.0f)
    {
        float sinRotation = sinf(rotation*DEG2RAD);
        float cosRotation = cosf(rotation*DEG2RAD);

        // NOTE: (-1, 1) is the range where origin.x, origin.y is inside the texture
        float rotateAboutX = sizeRatio.x*origin.x/2;
        float rotateAboutY = sizeRatio.y*origin.y/2;

        float xtvalue, ytvalue;
        float rotatedX, rotatedY;

        xtvalue = Vector3DotProduct(right, topLeft) - rotateAboutX; // Project points to x and y coordinates on the billboard plane
        ytvalue = Vector3DotProduct(up, topLeft) - rotateAboutY;
        rotatedX = xtvalue*cosRotation - ytvalue*sinRotation + rotateAboutX; // Rotate about the point origin
        rotatedY = xtvalue*sinRotation + ytvalue*cosRotation + rotateAboutY;
        topLeft = Vector3Add(Vector3Scale(up, rotatedY), Vector3Scale(right, rotatedX)); // Translate back to cartesian coordinates

        xtvalue = Vector3DotProduct(right, topRight) - rotateAboutX;
        ytvalue = Vector3DotProduct(up, topRight) - rotateAboutY;
        rotatedX = xtvalue*cosRotation - ytvalue*sinRotation + rotateAboutX;
        rotatedY = xtvalue*sinRotation + ytvalue*cosRotation + rotateAboutY;
        topRight = Vector3Add(Vector3Scale(up, rotatedY), Vector3Scale(right, rotatedX));

        xtvalue = Vector3DotProduct(right, bottomRight) - rotateAboutX;
        ytvalue = Vector3DotProduct(up, bottomRight) - rotateAboutY;
        rotatedX = xtvalue*cosRotation - ytvalue*sinRotation + rotateAboutX;
        rotatedY = xtvalue*sinRotation + ytvalue*cosRotation + rotateAboutY;
        bottomRight = Vector3Add(Vector3Scale(up, rotatedY), Vector3Scale(right, rotatedX));

        xtvalue = Vector3DotProduct(right, bottomLeft)-rotateAboutX;
        ytvalue = Vector3DotProduct(up, bottomLeft)-rotateAboutY;
        rotatedX = xtvalue*cosRotation - ytvalue*sinRotation + rotateAboutX;
        rotatedY = xtvalue*sinRotation + ytvalue*cosRotation + rotateAboutY;
        bottomLeft = Vector3Add(Vector3Scale(up, rotatedY), Vector3Scale(right, rotatedX));
    }

    // Translate points to the draw center (position)
    topLeft = Vector3Add(topLeft, position);
    topRight = Vector3Add(topRight, position);
    bottomRight = Vector3Add(bottomRight, position);
    bottomLeft = Vector3Add(bottomLeft, position);

    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    if (sizeRatio.x * sizeRatio.y >= 0.0f)
    {
        // Bottom-left corner for texture and quad
        rlTexCoord2f((float)source.x/texture.width, (float)source.y/texture.height);
        rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

        // Top-left corner for texture and quad
        rlTexCoord2f((float)source.x/texture.width, (float)(source.y + source.height)/texture.height);
        rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

        // Top-right corner for texture and quad
        rlTexCoord2f((float)(source.x + source.width)/texture.width, (float)(source.y + source.height)/texture.height);
        rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);

        // Bottom-right corner for texture and quad
        rlTexCoord2f((float)(source.x + source.width)/texture.width, (float)source.y/texture.height);
        rlVertex3f(topRight.x, topRight.y, topRight.z);
    }
    else
    {
        // Reverse vertex order if the size has only one negative dimension
        rlTexCoord2f((float)(source.x + source.width)/texture.width, (float)source.y/texture.height);
        rlVertex3f(topRight.x, topRight.y, topRight.z);

        rlTexCoord2f((float)(source.x + source.width)/texture.width, (float)(source.y + source.height)/texture.height);
        rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);

        rlTexCoord2f((float)source.x/texture.width, (float)(source.y + source.height)/texture.height);
        rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

        rlTexCoord2f((float)source.x/texture.width, (float)source.y/texture.height);
        rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
    }

    rlEnd();

    rlSetTexture(0);
}

void Renderer::BeginAlphaCutoff() {
    BeginShaderMode(alphaCutoffShader);
}

void Renderer::EndAlphaCutoff() {
    EndShaderMode();
}

void Renderer::RenderTriangle(Vector3 position, float size,float angle, Color color) {

    Vector3 camUp = GetCameraUp(camera.get());
    Vector3 camRight = GetCameraRight(camera.get());


    //TODO: Implement angle
    Vector3 axis = GetCameraForward(camera.get());
    camUp = Vector3RotateByAxisAngle(camUp,axis,angle);
    camRight = Vector3RotateByAxisAngle(camRight,axis,angle);


    Vector3 point1 = Vector3Add(position, Vector3Scale(camRight,size));
    Vector3 point3 = Vector3Add(position, Vector3Scale(camRight,-size));
    Vector3 point2 = Vector3Add(position, Vector3Scale(camUp,size));

    //Fill shape
    DrawTriangle3DWithRotation(point1,point2,point3,camRight,angle,FAKE_TRANSPARENT1);

    //Lines
    DrawLine3D(point1,point2,color);
    DrawLine3D(point2,point3,color);
    DrawLine3D(point3,point1,color);

}

void Renderer::DrawTriangle3DWithRotation(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 rotationAxis, float angle, Color color) {

    //rlRotatef(angle,rotationAxis.x,rotationAxis.y,rotationAxis.z);



    rlPushMatrix();
        rlBegin(RL_TRIANGLES);
        rlEnableBackfaceCulling();

            rlColor4ub(color.r, color.g, color.b, color.a);
            rlVertex3f(v1.x, v1.y, v1.z);
            rlVertex3f(v2.x, v2.y, v2.z);
            rlVertex3f(v3.x, v3.y, v3.z);
        rlEnd();
    rlPopMatrix();

}

void Renderer::RenderSphere(Vector3 position, float size, Color insideColor,Color lineColor) {

    float cut = 4;

    //DrawSphereWires(position,size,cut,cut,lineColor);
    DrawSphereEx(position,size,cut,cut,insideColor);
}
