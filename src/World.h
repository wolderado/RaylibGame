//
// Created by Salih on 12-Dec-23.
//

#ifndef SRC_WORLD_H
#define SRC_WORLD_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <map>
#include "GameObject.h"
#include "Asteroid.h"



using namespace std;

class World {
public:
    World();
    static World* GetInstance();
    void UpdateAll(float deltaTime);
    shared_ptr<GameObject> CreateNewObject();
    shared_ptr<GameObject> CreateNewAsteroid(Vector3 position,float maxSize = 100);
    void InitObject(shared_ptr<GameObject> target);
    void CheckCollision(GameObject* object);
    bool CheckCollisionPure(Vector3 position1,float size1, Vector3 position2,float size2);
    shared_ptr<GameObject> CheckBulletCollision(Vector3 bulletPosition);
    map<uint32_t, shared_ptr<GameObject>> activeGameObjects;
private:
    uint32_t nextObjectId = 0;
    static World* instance;



    void OnGameObjectDestroyed();
};


#endif //SRC_WORLD_H
