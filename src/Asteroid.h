//
// Created by Salih on 13-Dec-23.
//

#ifndef SRC_ASTEROID_H
#define SRC_ASTEROID_H

#include "GameObject.h"

class Asteroid : public GameObject {
public:
    Asteroid();
    void Update(float deltaTime) override;
private:
    Vector3 rndAngularMomentum;
};


#endif //SRC_ASTEROID_H
