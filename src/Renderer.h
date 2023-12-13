//
// Created by Salih on 10-Dec-23.
//

#ifndef SRC_RENDERER_H
#define SRC_RENDERER_H
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"
#include "Color.h"
#include "Player.h"


class Renderer {
public:
    void InitRenderer(Player* pla);
    void RenderBackground();
    void RenderAtmosphere();
    void Unload();
    bool IsVisible(Vector3 position);
private:
    float maxLineLength = 0.5f;
    Vector3 starPositions[1000];
    Texture backgroundTexture;
    shared_ptr<Camera> camera;
    shared_ptr<Player> player;
    void DrawDots();
};


#endif //SRC_RENDERER_H
