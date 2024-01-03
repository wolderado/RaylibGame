//
// Created by Salih on 03-Jan-24.
//

#ifndef SRC_BATTLEMANAGER_H
#define SRC_BATTLEMANAGER_H
#include <iostream>

#include "World.h"



class BattleManager {
public:
    void Init();
    void UpdateAI(float deltaTime);
    void ProcessAI(shared_ptr<Fighter> object);

private:
    World* worldInstance;

};


#endif //SRC_BATTLEMANAGER_H
