//
// Created by Salih on 22-Dec-23.
//

#ifndef SRC_PARTICLEMANAGER_H
#define SRC_PARTICLEMANAGER_H


#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "Globals.h"
#include "Renderer.h"
#include "GameObject.h"


using namespace std;


struct Particle
{
    bool isActive;
    Vector3 position;
    Vector3 direction;
    float angle;
    float speed;
    float size;
    Color tint;
    int particleIndex;
    int particleRowIndex;
    float lifeTime;
    float maxLifeTime;
    bool stopOverTime;
    bool isLineParticle;
    bool useLocalSpace;
    GameObject* parentObject;
    Vector3 localPosition;

    //default
    float defaultMaxSpeed;
};



class ParticleManager {
public:
    void UpdateAndRender(float deltaTime);
    static ParticleManager* GetInstance();
    Particle* CreateParticle();
    void CreateHitParticle(Vector3 position);
    void InitDefaults(Particle* particle);
    void CreateShootMuzzle(Vector3 localPosition,GameObject* parentObject);
private:
    const int MaxParticles = 1000;

    static ParticleManager* instance;

    //Pooling particles
    vector<Particle> activeParticles;
    stack<Particle> inactiveParticles;
};


#endif //SRC_PARTICLEMANAGER_H
