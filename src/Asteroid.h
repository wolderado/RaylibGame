//
// Created by Salih on 13-Dec-23.
//

#ifndef SRC_ASTEROID_H
#define SRC_ASTEROID_H

#include "GameObject.h"
#include "World.h"
#include "ParticleManager.h"

class Asteroid : public GameObject {
public:
    explicit Asteroid(float asteroidSize);
    void Update(float deltaTime) override;
    void Destroy() override;
private:
    Vector3 rndAngularMomentum;
    float asteroidSize = 1;
};


#endif //SRC_ASTEROID_H
