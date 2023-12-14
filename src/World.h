//
// Created by Salih on 12-Dec-23.
//

#ifndef SRC_WORLD_H
#define SRC_WORLD_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include "GameObject.h"
#include "Asteroid.h"
#include "Player.h"


using namespace std;

class World {
public:
    World();
    static World* GetInstance();
    void UpdateAll(float deltaTime);
    shared_ptr<GameObject> CreateNewObject();
    shared_ptr<GameObject> CreateNewAsteroid(Vector3 position);
    shared_ptr<Player> CreatePlayer();


private:
    Player player;
    static World* instance;
    vector<shared_ptr<GameObject>> activeGameObjects;
    void InitObject(shared_ptr<GameObject> target);
    void CheckCollision(GameObject* object);

};


#endif //SRC_WORLD_H
