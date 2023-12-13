//
// Created by Salih on 10-Dec-23.
//

#ifndef SRC_RENDERER_H
#define SRC_RENDERER_H
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"
#include "../../raylib/src/rlgl.h"
#include "Color.h"
#include "Player.h"


class Renderer {
public:


    Renderer();
    static Renderer* GetInstance();
    void InitRenderer(Player* pla);
    void RenderBackground();
    void RenderAtmosphere();
    void Unload();
    bool IsVisible(Vector3 position);
    bool IsVisible(Vector3 position,float customDotValue);
    void RenderModelWire(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);
    void RenderModel(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);
    void RenderModelWithWires(Model targetModel,Vector3 position, Vector3 rotation, Vector3 scale, Color color);

private:
    static Renderer* instance;
    float maxLineLength = 0.5f;
    const float ModelsCullDotValue = -0.3f;
    const float LOD1Distance = 50;
    const float LOD2Distance = 100;

    Vector3 starPositions[1000];
    Texture backgroundTexture;
    shared_ptr<Camera> camera;
    shared_ptr<Player> player;
    void DrawDots();
    Model GenericLODModel;
};


#endif //SRC_RENDERER_H
