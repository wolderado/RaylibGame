//
// Created by Salih on 12-Dec-23.
//

#ifndef SRC_GAMEOBJECT_H
#define SRC_GAMEOBJECT_H

#include <string>
#include <iostream>

#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"

#include "Globals.h"
#include "Color.h"
#include "Renderer.h"



enum ObjectRenderType{
    Invisible,
    RealMesh,
    BillboardTexture
};


class GameObject {
public:
    std::string Name = "UNIDENTIFIED OBJECT";
    Vector3 Position;
    Vector3 Rotation;
    Vector3 Scale;



    GameObject();
    virtual void Update(float deltaTime);
    virtual void Render(float deltaTime);
    virtual void Hurt(float damage);
    virtual void SetModel(Model model);
    virtual void Destroy();
    TEAM GetTeam();
    void SetTeam(TEAM newTeam);


protected:
    TEAM myTeam = TEAM_NEUTRAL;
    float health{};
    Model myModel;
    ObjectRenderType myRenderType = Invisible;
    Color myColor;
};




#endif //SRC_GAMEOBJECT_H
