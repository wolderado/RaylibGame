//
// Created by Salih on 13-Dec-23.
//

#include "Asteroid.h"

void Asteroid::Update(float deltaTime) {

    Rotation = Vector3Add(Rotation, Vector3Scale(rndAngularMomentum,deltaTime));

    GameObject::Update(deltaTime);
}

Asteroid::Asteroid() {
    rndAngularMomentum =
            {GetRandomValue(-100, 100) * 0.01f,
             GetRandomValue(-100, 100) * 0.01f,
             GetRandomValue(-100, 100) * 0.01f};
}