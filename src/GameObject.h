//
// Created by Salih on 12-Dec-23.
//

#ifndef SRC_GAMEOBJECT_H
#define SRC_GAMEOBJECT_H

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>

#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"

#include "Globals.h"
#include "Color.h"
#include "Renderer.h"
#include "SoundManager.h"



enum ObjectRenderType{
    Invisible,
    RealMesh,
    BillboardTexture
};

enum TEAM{
    TEAM_NEUTRAL = 0,
    TEAM_PLAYER = 1,
    TEAM_ENEMY = 2,
    TEAM_ALLY = 3
};


class GameObject {
public:
    const float FlashDuration = 0.13f;

    std::string Name = "UNIDENTIFIED OBJECT";
    Vector3 Position = {0,0,0};
    Quaternion Rotation = QuaternionIdentity();
    Vector3 Scale = {1,1,1};
    bool CanCollide = true;
    bool IgnoresAllCollisions = false;
    bool ShowHealthBar = false;
    float CollisionSize = 1.0f;
    float Mass = 1;
    uint32_t WorldID = 0;
    float LastHurtTime = 0;
    tuple<int,int,int> GridIndex = {0,0,0};



    GameObject();
    virtual void Update(float deltaTime);
    virtual void LateUpdate(float deltaTime);
    virtual void Render(float deltaTime);
    virtual void Hurt(float damage);
    virtual void Heal(float amount);
    virtual float GetHealth();
    virtual void SetModel(Model model);
    virtual void Destroy();
    virtual void OnCollision(GameObject* otherObject,Vector3 collisionTotalVelocity);  //Collision event triggered by world interactions
    virtual void OnInit();


    TEAM GetTeam();
    void SetTeam(TEAM newTeam);
    Vector3 GetVelocity() { return currentVelocity; }
    void SetVelocity(Vector3 newVelocity);
    Vector3 GetVelocityNormalized() { return Vector3Normalize(currentVelocity); }
    void SetHealth(float newMaxHealth);
    Model GetModel() { return myModel; }
    void AddTag(const string tag) { tags.insert(tag); }
    bool HasTag(const string tag) { return tags.find(tag) != tags.end(); }
    Vector3 GetForward();
    Vector3 GetRight();
    Vector3 GetUp();
    bool IsEnabled();
    float GetHealthRatio() { return health / maxHealth; };
    float GetMaxHealth() { return maxHealth; };


protected:
    TEAM myTeam = TEAM_NEUTRAL;
    float health = 1;
    float maxHealth = 1;
    Model myModel = {0};
    ObjectRenderType myRenderType = Invisible;
    Color myColor = WHITE;
    Vector3 currentVelocity = {0,0,0};
    bool enabled = true;
    unordered_set<string> tags = {};
    Color hurtFlashColor = PALETTE_RED1;
    float lastCollisionTime = 0;
};




#endif //SRC_GAMEOBJECT_H
