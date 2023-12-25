//
// Created by Salih on 13-Dec-23.
//

#include "Asteroid.h"

void Asteroid::Update(float deltaTime) {

    Rotation = Vector3Add(Rotation, Vector3Scale(rndAngularMomentum,deltaTime));


    GameObject::Update(deltaTime);
}

Asteroid::Asteroid(float size){

    float rotateSpeed= 0.005f;
    asteroidSize = size;
    Name = "Asteroid";
    //cout << "Creating asteroid with size " << asteroidSize << endl;

    rndAngularMomentum =
            {GetRandomValue(-100, 100) * rotateSpeed,
             GetRandomValue(-100, 100) * rotateSpeed,
             GetRandomValue(-100, 100) * rotateSpeed};
}

void Asteroid::Destroy() {
    GameObject::Destroy();

    ParticleManager::GetInstance()->CreateAsteroidExplosion(Position,asteroidSize);

    if(asteroidSize < 25)
        return;

    //cout << asteroidSize << " sized asteroid destroyed, creating 2 new ones" << endl;


    float newAsteroidSize = asteroidSize/2.0f;
    int minForceMagnitude = 60;
    int maxForceMagnitude = 60;
    float finalForce = GetRandomValue(minForceMagnitude,maxForceMagnitude) * 0.01f;

    Vector3 rndDirection = Utility::GetRandomDirection();

    for (int i = 0; i < 2; ++i) {
        shared_ptr<GameObject> asteroid = World::GetInstance()->CreateNewAsteroid(Position,newAsteroidSize);
        asteroid->SetVelocity(Vector3Scale(rndDirection, finalForce));
    }


}
