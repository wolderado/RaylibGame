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
#include <tuple>
#include "GameObject.h"
#include "Asteroid.h"
#include "Fighter.h"
#include "Player.h"
#include "CollisionGrid.h"
#include "ParticleManager.h"
#include "Scrap.h"




using namespace std;

class World {
public:
    World();
    static World* GetInstance();
    void UpdateAll(float deltaTime);
    shared_ptr<GameObject> CreateNewObject();
    shared_ptr<GameObject> CreateNewAsteroid(Vector3 position,float maxSize = 100);
    shared_ptr<GameObject> CreateNewFighter(TEAM team, Vector3 position);
    shared_ptr<GameObject> CreateNewScrap(Vector3 position,float amount);
    void InitObject(shared_ptr<GameObject> target);
    void CheckCollision(GameObject* object);
    bool CheckCollisionSingle(Vector3 position1, float size1, Vector3 position2, float size2);
    tuple<bool,GameObject*> CheckBulletCollision(Vector3 bulletPosition,TEAM bulletTeam);
    void GenerateWorld();
    Vector3 ConvertGridPosToRealPos(std::tuple<int,int,int> gridIndex);
    CollisionGrid* GetGrid(int x, int y, int z);
    CollisionGrid* GetGridFromRealPos(Vector3 realPos);
    tuple<int,int,int> GetGridPosFromRealPos(Vector3 realPos);
    void ResumeWorld() { worldStopped = false; };
    void StopWorld() { worldStopped = true; };
    bool IsWorldStopped() { return worldStopped; };

    using newObjectCreation = function<void(shared_ptr<GameObject>)>;
    newObjectCreation OnNewObjectCreated;

    int AllyFighterCount = 0;
    int EnemyFighterCount = 0;
    int AsteroidCount = 0;
    bool IsWorldReady = false;
    int DEBUG_BulletCount = 0;
    int DEBUG_DestroyedObjectCount = 0;


    map<tuple<int,int,int>,CollisionGrid> Grid;
    map<uint32_t, shared_ptr<GameObject>> activeGameObjects;
private:
    uint32_t nextObjectId = 0;
    static World* instance;
    bool worldStopped = false;


    void DebugHurtClosestObject();
    void UpdateGridForObject(shared_ptr<GameObject> object);
    void OnGameObjectDestroyed(shared_ptr<GameObject> destroyedObject);

};


#endif //SRC_WORLD_H
