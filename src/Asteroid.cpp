//
// Created by Salih on 13-Dec-23.
//

#include "Asteroid.h"

void Asteroid::Update(float deltaTime) {


    //Disable collision at the first few seconds
    if(disableCollisionTimer > 0) {
        disableCollisionTimer -= deltaTime;
        CanCollide = false;
        if(disableCollisionTimer <= 0)
            CanCollide = true;
    }

    rndAngle += deltaTime * rotateSpeed;
    Rotation = QuaternionFromEuler(rndAxis.x,rndAxis.y + rndAngle,rndAxis.z);
/*

    if(rndAngle * RAD2DEG > 360 || rndAngle * RAD2DEG < -360)
        rndAngle = 0;


*/

    //ParticleManager::GetInstance()->CreateShootMuzzle((Vector3){0,1.0f,0}, this);

    GameObject::Update(deltaTime);

    if(Utility::IsInsideMapArea(Position) == false)
        Hurt(9999);
}

Asteroid::Asteroid(float size) {

    AsteroidSize = size;
    Name = "Asteroid";
    disableCollisionTimer = 2.0f;
    CanCollide = false;
    rotateSpeed = GetRandomValue(-100,100) * 0.005f;

    rndAxis = {GetRandomValue(0, 360) * DEG2RAD,
               GetRandomValue(0, 360) * DEG2RAD,
               GetRandomValue(0, 360) * DEG2RAD};
}

void Asteroid::Destroy() {
    GameObject::Destroy();

    ParticleManager::GetInstance()->CreateAsteroidExplosion(Position,AsteroidSize);
}
