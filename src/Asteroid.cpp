//
// Created by Salih on 13-Dec-23.
//

#include "Asteroid.h"

void Asteroid::Update(float deltaTime) {

    Rotation = Vector3Add(Rotation, Vector3Scale(rndAngularMomentum,deltaTime));


    GameObject::Update(deltaTime);
}

Asteroid::Asteroid() {
    float rotateSpeed= 0.005f;

    rndAngularMomentum =
            {GetRandomValue(-100, 100) * rotateSpeed,
             GetRandomValue(-100, 100) * rotateSpeed,
             GetRandomValue(-100, 100) * rotateSpeed};
}