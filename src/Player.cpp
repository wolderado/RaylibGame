//
// Created by Salih on 09-Dec-23.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "Player.h"


#endif

void Player::Init() {
    position = 0;

    std::cout << "No need to store this string";
}

void Player::Update(float deltaTime) {

    position += deltaTime;

    DrawText("PLAYER",GetRenderWidth()/2 - 200, GetRenderHeight()/2 + round(sin(position) * 200.0),50,PURPLE);
}
