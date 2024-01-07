//
// Created by Salih on 13-Dec-23.
//

#ifndef SRC_ASTEROID_H
#define SRC_ASTEROID_H

#include "GameObject.h"
#include "ParticleManager.h"

class Asteroid : public GameObject {
public:
    explicit Asteroid(float asteroidSize);
    void Update(float deltaTime) override;
    void Destroy() override;
    float AsteroidSize = 1;
private:
    Vector3 rndAxis = {0,0,0};
    float rndAngle = 0;
    float rotateSpeed = 0;
    float disableCollisionTimer = 2.0f;

};


#endif //SRC_ASTEROID_H
