//
// Created by Salih on 10-Dec-23.
//

#ifndef SRC_RENDERER_H
#define SRC_RENDERER_H
#include <iostream>
#include <memory>
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"
#include "../../raylib/src/rlgl.h"
#include "Color.h"
#include "Globals.h"



using namespace std;

struct ScrapVertexPoint {
    Vector3 position;
    Vector3 direction;
    Color lineColor;
    float speed;
};


class Renderer {
public:
    Renderer();
    static Renderer* GetInstance();
    void InitRenderer(Camera* cam);
    void RenderBackground();
    void RenderAtmosphere(float cameraVelocityRatio,Vector3 cameraVelocity);
    void Update(float deltaTime);
    void UpdateScrapMesh(float deltaTime);
    void Unload();
    bool IsVisible(Vector3 position);
    bool IsVisible(Vector3 position,float customDotValue);
    void RenderModelWire(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);
    void RenderModel(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);
    void RenderModelWithWires(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color,bool ignoreOptimizations = false);
    void RenderBillboard(int spriteRowIndex,int spriteIndex,Vector3 position, float size,float rotation,Color simplifiedColor);
    void RenderTriangle(Vector3 position,float size,float angle,Color color);
    void RenderSphere(Vector3 position,float size, Color insideColor,Color lineColor);
    void RenderScrap(Vector3 position,float size,bool is2D = false);
    void BeginAlphaCutoff();
    void EndAlphaCutoff();

private:
    static Renderer* instance;
    const float maxLineLength = 0.5f;
    const float ModelsCullDotValue = -0.3f;
    const float LOD1Distance = 500;
    const float CullDistance = 700;
    const float LODScaleDistanceOffset = 10;
    const float scrapVertexMoveSpeed = 8.0f;
    const float scrapVertexMoveSpeedMax = 12.0f;

    Texture skyboxTexture;
    Texture billboardTextureSheet;
    shared_ptr<Camera> camera;
    Model genericLODModel;
    Model skyboxModel;
    Shader alphaCutoffShader;
    ScrapVertexPoint scrapPoints[15];

    void DrawDots(float cameraVelocityRatio,Vector3 cameraVelocity);
    void DrawBillboard(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector3 up, Vector2 size, Vector2 origin, float rotation, Color tint);
    void DrawTriangle3DWithRotation(Vector3 v1,Vector3 v2,Vector3 v3,Vector3 rotationAxis,float angle,Color color);
};


#endif //SRC_RENDERER_H
