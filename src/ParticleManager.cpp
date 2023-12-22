//
// Created by Salih on 22-Dec-23.
//

#include "ParticleManager.h"


ParticleManager* ParticleManager::instance = nullptr;


ParticleManager* ParticleManager::GetInstance() {
    if(instance == nullptr)
        instance = new ParticleManager();

    return instance;
}

void ParticleManager::UpdateAndRender(float deltaTime) {

    Renderer* renderer = Renderer::GetInstance();
/*

    cout << activeParticles.size() << endl;
*/

    for (int i = 0; i < activeParticles.size(); i++) {
        Particle *particle = &activeParticles[i];

        float lifeRatio = particle->lifeTime / particle->maxLifeTime;
        Vector3 velocity = Vector3Scale(particle->direction, particle->speed * deltaTime);

        if(particle->useLocalSpace)
        {
            Matrix worldMatrix = MatrixTranslate(particle->parentObject->Position.x, particle->parentObject->Position.y, particle->parentObject->Position.z);
            worldMatrix = MatrixMultiply(worldMatrix, MatrixRotateXYZ((Vector3){particle->parentObject->Rotation.x ,particle->parentObject->Rotation.y,particle->parentObject->Rotation.z}));

            Matrix localMatrix = MatrixTranslate(particle->localPosition.x, particle->localPosition.y, particle->localPosition.z);
            //localMatrix = MatrixMultiply(localMatrix, MatrixRotateXYZ((Vector3){0,0,particle->angle}));

            Matrix finalMatrix = MatrixMultiply(worldMatrix,localMatrix);

            particle->position = Vector3Transform(Vector3Zero(),finalMatrix);
        }

        Vector3 newPos = Vector3Add(particle->position, velocity);
        particle->position = newPos;

        //Air Friction
        if (particle->stopOverTime) {
            particle->speed = Lerp(particle->speed, 0.0f, lifeRatio);
        }


        particle->lifeTime += deltaTime;
        if (particle->lifeTime > particle->maxLifeTime) {
            particle->isActive = false;
            inactiveParticles.push(*particle);
            activeParticles.erase(activeParticles.begin() + i);
            continue;
        }


        if (particle->isLineParticle == true)
        {
            float speedRatio = particle->speed / particle->defaultMaxSpeed;
            Vector3 endPos = Vector3Add(particle->position, Vector3Scale(particle->direction,0.1f * particle->size * speedRatio));
            DrawLine3D(particle->position,endPos,particle->tint);
        }
        else {
            renderer->RenderBillboard(particle->particleRowIndex,
                                      particle->particleIndex,
                                      particle->position,
                                      particle->size,
                                      particle->angle);

        }
    }
}



Particle *ParticleManager::CreateParticle() {

    Particle newParticle;

    bool particleLimitReached = false;


    if(inactiveParticles.size() > 0)
    {
        newParticle = inactiveParticles.top();
        inactiveParticles.pop();
    }else {

        //Start using older particles if limit is reached
        if(activeParticles.size() >= MaxParticles) {
            newParticle = activeParticles.front();
            activeParticles.erase(activeParticles.begin());
        }
        else
            newParticle = Particle();
    }

    newParticle.position = Vector3Zero();
    newParticle.angle = GetRandomValue(0,360);
    newParticle.size = 1;
    newParticle.lifeTime = 0.0f;
    newParticle.maxLifeTime = 2.0f;
    newParticle.isLineParticle = false;
    newParticle.stopOverTime = false;
    newParticle.isActive = true;
    newParticle.tint = WHITE;
    newParticle.useLocalSpace = false;
    activeParticles.push_back(newParticle);

    return &activeParticles[activeParticles.size() - 1];
}


void ParticleManager::InitDefaults(Particle *particle) {
    particle->defaultMaxSpeed = particle->speed;
}

void ParticleManager::CreateHitParticle(Vector3 position) {


    for (int i = 0; i < 3; ++i) {
        Particle* newParticle = CreateParticle();
/*        newParticle->particleRowIndex = 1;
        newParticle->particleIndex = GetRandomValue(0,7);*/
        newParticle->position = position;
        newParticle->speed = GetRandomValue(1,20);
        newParticle->direction = Vector3Normalize(Vector3{(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100)});
        newParticle->maxLifeTime = GetRandomValue(40,200) * 0.01f;
        newParticle->size = 10;
        newParticle->stopOverTime = true;
        newParticle->isLineParticle = true;
        newParticle->tint = Utility::LerpColor(PALETTE_YELLOW1,PALETTE_RED1,GetRandomValue(0,100) * 0.01f);
        InitDefaults(newParticle);
    }


    Particle* boomParticle = CreateParticle();
    boomParticle->particleRowIndex = 2;
    boomParticle->particleIndex = GetRandomValue(0,2);
    boomParticle->position = position;
    boomParticle->speed = 0;
    boomParticle->maxLifeTime = 0.15f;
    boomParticle->size = 1;
    InitDefaults(boomParticle);
}


void ParticleManager::CreateShootMuzzle(Vector3 localPosition, GameObject* parentObject) {

    Particle* boomParticle = CreateParticle();
    boomParticle->particleRowIndex = 2;
    boomParticle->particleIndex = GetRandomValue(0,2);
    boomParticle->position = localPosition;
    boomParticle->localPosition = localPosition;
    boomParticle->useLocalSpace = true;
    boomParticle->parentObject = parentObject;
    boomParticle->speed = 0;
    boomParticle->maxLifeTime = 0.15f;
    boomParticle->size = 0.75f;
    InitDefaults(boomParticle);
}
