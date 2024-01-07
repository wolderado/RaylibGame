//
// Created by Salih on 20-Dec-23.
//

#ifndef SRC_BULLETMANAGER_H
#define SRC_BULLETMANAGER_H

#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "Globals.h"
#include "Renderer.h"
#include "World.h"
#include "ParticleManager.h"


using namespace std;


struct Bullet
{
    Vector3 position;
    Vector3 direction;
    float speed ;
    int spriteIndex;
    float lifeTime;
    float maxLifeTime;
    bool isActive;
    float damage;
    TEAM sourceTeam;
};

class BulletManager {
public:
    BulletManager();
    void Init();
    static BulletManager* GetInstance();
    void UpdateAndRender(float deltaTime);
    void CreateBullet(Vector3 position,Vector3 direction,float damage,int spriteIndex,TEAM sourceTeam);
    void BulletHit(int index,GameObject* target);



private:
    static BulletManager* instance;
    shared_ptr<Camera> gameCamera;
    Renderer* renderer;

    //Pooling bullets
    vector<Bullet> activeBullets;
    stack<Bullet> inactiveBullets;
    void EraseBulletAt(int index);

};




#endif //SRC_BULLETMANAGER_H
