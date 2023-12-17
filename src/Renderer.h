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


class Renderer {
public:


    Renderer();
    static Renderer* GetInstance();
    void InitRenderer(Camera* cam);
    void RenderBackground();
    void RenderAtmosphere(float cameraVelocityRatio,Vector3 cameraVelocity);
    void Unload();
    bool IsVisible(Vector3 position);
    bool IsVisible(Vector3 position,float customDotValue);
    void RenderModelWire(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);
    void RenderModel(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);
    void RenderModelWithWires(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color,bool ignoreOptimizations = false);

private:
    static Renderer* instance;
    float maxLineLength = 0.5f;
    const float ModelsCullDotValue = -0.3f;
    const float LOD1Distance = 500;
    const float CullDistance = 700;
    const float LODScaleDistanceOffset = 10;

    Texture skyboxTexture;
    shared_ptr<Camera> camera;
    //shared_ptr<Player> player;
    void DrawDots(float cameraVelocityRatio,Vector3 cameraVelocity);
    Model genericLODModel;
    Model skyboxModel;
};


#endif //SRC_RENDERER_H
