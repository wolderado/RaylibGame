//
// Created by Salih on 04-Jan-24.
//

#ifndef SRC_COLLISIONGRID_H
#define SRC_COLLISIONGRID_H

#include "GameObject.h"


class CollisionGrid {
public:
    static const float CellSize;

    bool Initialized = false;
    int x;
    int y;
    int z;

    vector<GameObject*> myObjects;
    vector<int> test;

    void AddObject(shared_ptr<GameObject> gameObject);
    void RemoveObject(shared_ptr<GameObject> gameObject);
    CollisionGrid(int x, int y, int z);
    CollisionGrid();

};


#endif //SRC_COLLISIONGRID_H
