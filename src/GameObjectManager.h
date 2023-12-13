//
// Created by Salih on 12-Dec-23.
//

#ifndef SRC_GAMEOBJECTMANAGER_H
#define SRC_GAMEOBJECTMANAGER_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include "GameObject.h"

using namespace std;

class GameObjectManager {
public:
    void UpdateAll(float deltaTime);
    shared_ptr<GameObject> CreateNewObject();
    shared_ptr<GameObject> CreateNewAsteroid(Vector3 position);
private:
    vector<shared_ptr<GameObject>> activeGameObjects;

};


#endif //SRC_GAMEOBJECTMANAGER_H
