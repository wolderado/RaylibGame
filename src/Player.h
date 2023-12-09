//
// Created by Salih on 09-Dec-23.
//class

#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
#include <cmath>
#include "../../raylib/src/raylib.h"


#endif // PLAYER_H


class Player {
public:
    float position;
    void Init();
    void Update(float deltaTime);
};
