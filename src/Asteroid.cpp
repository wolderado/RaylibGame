//
// Created by Salih on 13-Dec-23.
//

#include "Asteroid.h"

void Asteroid::Update(float deltaTime) {

    rndAngle += deltaTime * rotateSpeed;
    Rotation = QuaternionFromEuler(rndAxis.x,rndAxis.y + rndAngle,rndAxis.z);

    if(rndAngle * RAD2DEG > 360 || rndAngle * RAD2DEG < -360)
        rndAngle = 0;



    //ParticleManager::GetInstance()->CreateShootMuzzle((Vector3){0,1.0f,0}, this);

    GameObject::Update(deltaTime);
}

Asteroid::Asteroid(float size) : GameObject() {

    float rotateSpeed= 0.005f;
    AsteroidSize = size;
    Name = "Asteroid";
    //cout << "Creating asteroid with size " << asteroidSize << endl;


    rndAngle = 0;
    rotateSpeed = GetRandomValue(-100,100) * 0.005f;

    rndAxis = {GetRandomValue(0, 360) * DEG2RAD,
                          GetRandomValue(0, 360) * DEG2RAD,
                          GetRandomValue(0, 360) * DEG2RAD};
}

void Asteroid::Destroy() {
    GameObject::Destroy();

    ParticleManager::GetInstance()->CreateAsteroidExplosion(Position,AsteroidSize);
}
