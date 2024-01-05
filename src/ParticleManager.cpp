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


    //cout << activeParticles.size() << endl;


/*    //Stress test billboard
    for (int x = -MapSizeX; x < MapSizeX; x+=10) {
        for(int y = -MapSizeY; y< MapSizeY; y+=10){
            for(int z = -MapSizeZ; z < MapSizeZ; z+=10){

                renderer->RenderBillboard(0,0,(Vector3){(float)x,(float)y,(float)z},1.0f,0,PALETTE_GRAY2);
            }
        }
    }*/

    for (int i = 0; i < activeParticles.size(); i++) {
        Particle *particle = &activeParticles[i];

        float lifeRatio = particle->lifeTime / particle->maxLifeTime;
        Vector3 velocity = Vector3Scale(particle->direction, particle->speed * deltaTime);

        if(particle->useLocalSpace)
        {
            Vector3 pos = particle->localPosition;

            pos = Vector3RotateByAxisAngle(pos,(Vector3){0,1,0},particle->parentObject->Rotation.y);
            pos = Vector3RotateByAxisAngle(pos,(Vector3){1,0,0},particle->parentObject->Rotation.x);
            pos = Vector3RotateByAxisAngle(pos,(Vector3){0,0,1},particle->parentObject->Rotation.z);

            pos = Vector3Add(pos,particle->parentObject->Position);

            particle->position = pos;

        }

        Vector3 newPos = Vector3Add(particle->position, velocity);
        particle->position = newPos;

        //Space Friction
        if (particle->stopOverTime) {
            particle->speed = Lerp(particle->defaultMaxSpeed, 0.0f, lifeRatio);
        }

        if(particle->scaleDownOverTime) {
            particle->size = Lerp(particle->defaultSize, 0.0f, lifeRatio);
        }

        if(particle->fadeTintDownOverTime) {
            Color defTint = particle->defaultTint;
            particle->tint = Utility::LerpColor(defTint,(Color){defTint.r,defTint.g,defTint.b,0},lifeRatio);
        }

        if(particle->scaleTargetOverTime > 0.0001f) {
            //Ease Out Cubic
            float curvedT = 1 - pow(1 - lifeRatio, 3);
            particle->size = Lerp(particle->defaultSize, particle->scaleTargetOverTime, curvedT);
        }

        particle->lifeTime += deltaTime;
        if (particle->lifeTime > particle->maxLifeTime) {
            particle->isActive = false;
            inactiveParticles.push(*particle);
            activeParticles.erase(activeParticles.begin() + i);
            continue;
        }



        //Render Particles
        if (particle->particleType == ParticleType::Line)
        {
            float speedRatio = particle->speed / particle->defaultMaxSpeed;
            Vector3 endPos = Vector3Add(particle->position, Vector3Scale(particle->direction,0.1f * particle->size * speedRatio));
            DrawLine3D(particle->position,endPos,particle->tint);
        }
        else if(particle->particleType == ParticleType::Triangle)
        {
            renderer->RenderTriangle(particle->position,particle->size,particle->angle,particle->tint);
        }
        else if(particle->particleType == ParticleType::Sphere)
        {
            Color lineColor = (Color){particle->tint.r,particle->tint.g,particle->tint.b,255};
            renderer->RenderSphere(particle->position,particle->size,particle->tint,particle->tint);
        }
        else
        {
            renderer->RenderBillboard(particle->particleRowIndex,
                                      particle->particleIndex,
                                      particle->position,
                                      particle->size,
                                      particle->angle,
                                      particle->distantColor);
        }
    }
}



#pragma region Ready Particle Spawns

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
    newParticle.particleType = ParticleType::Sprite;
    newParticle.stopOverTime = false;
    newParticle.isActive = true;
    newParticle.tint = WHITE;
    newParticle.useLocalSpace = false;
    newParticle.particleType = ParticleType::Sprite;
    newParticle.scaleDownOverTime = false;
    newParticle.fadeTintDownOverTime = false;
    newParticle.scaleTargetOverTime = 0.0f;
    newParticle.parentObject = nullptr;
    newParticle.localPosition = Vector3Zero();
    newParticle.distantColor = PALETTE_GRAY4;
    activeParticles.push_back(newParticle);

    return &activeParticles[activeParticles.size() - 1];
}


void ParticleManager::InitDefaults(Particle *particle) {
    particle->defaultMaxSpeed = particle->speed;
    particle->defaultSize = particle->size;
    particle->defaultTint = particle->tint;
}

void ParticleManager::CreateHitParticle(Vector3 position) {


    for (int i = 0; i < 3; ++i) {
        Particle* newParticle = CreateParticle();
/*        newParticle->particleRowIndex = 1;
        newParticle->particleIndex = GetRandomValue(0,7);*/
        newParticle->position = position;
        newParticle->speed = GetRandomValue(1,20);
        newParticle->direction = Vector3Normalize(Vector3{(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100)});
        newParticle->maxLifeTime = GetRandomValue(30,70) * 0.01f;
        newParticle->size = 10;
        newParticle->stopOverTime = true;
        newParticle->particleType = ParticleType::Line;
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
    boomParticle->useLocalSpace = false;
    boomParticle->parentObject = parentObject;
    boomParticle->speed = 0;
    boomParticle->maxLifeTime = 0.15f;
    boomParticle->size = 0.75f;
    InitDefaults(boomParticle);
}

void ParticleManager::CreateAsteroidExplosion(Vector3 position,float asteroidSize) {

    //Lines
    for (int i = 0; i < 30; ++i) {
        Particle* newParticle = CreateParticle();
        newParticle->position = Vector3Add(position, Vector3Scale(Utility::GetRandomDirection(),asteroidSize*0.01f));
        newParticle->speed = GetRandomValue(15,50);
        newParticle->direction = Vector3Normalize(Vector3{(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100)});
        newParticle->maxLifeTime = GetRandomValue(30,75) * 0.01f;
        newParticle->size = 60;
        newParticle->stopOverTime = true;
        newParticle->particleType = ParticleType::Line;
        newParticle->tint = Utility::LerpColor(PALETTE_GRAY5,PALETTE_GRAY2,GetRandomValue(0,100) * 0.01f);
        InitDefaults(newParticle);
    }

    //Triangles
    for (int i = 0; i < asteroidSize / 2; ++i) {
        Particle* newParticle = CreateParticle();
        newParticle->particleRowIndex = 1;
        newParticle->particleIndex = GetRandomValue(0,7);
        newParticle->position = Vector3Add(position, Vector3Scale(Utility::GetRandomDirection(),asteroidSize*0.01f));
        newParticle->speed = GetRandomValue(0,20);
        newParticle->direction = Vector3Normalize(Vector3{(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100)});
        newParticle->maxLifeTime = GetRandomValue(100,300) * 0.01f;
        newParticle->size = asteroidSize * 0.025f;
        newParticle->angle = GetRandomValue(0,360);
        newParticle->scaleDownOverTime = true;
        newParticle->stopOverTime = true;
        newParticle->tint = PALETTE_GRAY3;
        newParticle->particleType = ParticleType::Triangle;
        InitDefaults(newParticle);
    }

    //Flash Particle
    Particle* boomParticle = CreateParticle();
    boomParticle->position = position;
    boomParticle->speed = 0;
    boomParticle->maxLifeTime = 0.075f;
    boomParticle->size = asteroidSize * 0.1f;
    boomParticle->particleType = ParticleType::Sphere;
    boomParticle->scaleTargetOverTime = asteroidSize * 0.07f;
    boomParticle->tint = PALETTE_YELLOW1;
    boomParticle->fadeTintDownOverTime = true;
    InitDefaults(boomParticle);
}



void ParticleManager::CreateShipExplosion(Vector3 position,TEAM team) {

    //Lines
    for (int i = 0; i < 30; ++i) {
        Particle* newParticle = CreateParticle();
        newParticle->position = position;
        newParticle->speed = GetRandomValue(30,90);
        newParticle->direction = Vector3Normalize(Vector3{(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100)});
        newParticle->maxLifeTime = GetRandomValue(40,110) * 0.01f;
        newParticle->size = 60;
        newParticle->stopOverTime = true;
        newParticle->particleType = ParticleType::Line;
        Color targetTint = team == TEAM_ALLY ? PALETTE_GREEN2 : PALETTE_RED2;
        newParticle->tint = Utility::LerpColor(PALETTE_YELLOW2,targetTint,GetRandomValue(0,100) * 0.01f);
        InitDefaults(newParticle);
    }

    //Triangles
    for (int i = 0; i < 30; ++i) {
        Particle* newParticle = CreateParticle();
        newParticle->particleRowIndex = 1;
        newParticle->particleIndex = GetRandomValue(0,7);
        newParticle->position = position;
        newParticle->speed = GetRandomValue(0,20);
        newParticle->direction = Vector3Normalize(Vector3{(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100),(float)GetRandomValue(-100,100)});
        newParticle->maxLifeTime = GetRandomValue(200,600) * 0.01f;
        newParticle->size = 1.25f;
        newParticle->angle = GetRandomValue(0,360);
        newParticle->scaleDownOverTime = true;
        newParticle->stopOverTime = true;
        newParticle->tint = team == TEAM_ALLY ? PALETTE_GREEN2 : PALETTE_RED2;;
        newParticle->particleType = ParticleType::Triangle;
        InitDefaults(newParticle);
    }

    //Flash Particle
    Particle* boomParticle = CreateParticle();
    boomParticle->position = position;
    boomParticle->speed = 0;
    boomParticle->maxLifeTime = 0.075f;
    boomParticle->size = 5;
    boomParticle->particleType = ParticleType::Sphere;
    boomParticle->scaleTargetOverTime = 3.5f;
    boomParticle->tint = PALETTE_YELLOW1;
    boomParticle->fadeTintDownOverTime = true;
    InitDefaults(boomParticle);

    //Smoke Sphere Particle
    Particle* sphereParticle = CreateParticle();
    sphereParticle->position = position;
    sphereParticle->speed = 0;
    sphereParticle->maxLifeTime = 1.0f;
    sphereParticle->size = 2;
    sphereParticle->particleType = ParticleType::Sphere;
    sphereParticle->scaleTargetOverTime = 10.5f;
    sphereParticle->tint = PALETTE_YELLOW2;
    sphereParticle->fadeTintDownOverTime = true;
    InitDefaults(sphereParticle);
}

#pragma endregion