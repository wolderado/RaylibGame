//
// Created by Salih on 04-Jan-24.
//

#include "CollisionGrid.h"

const float CollisionGrid::CellSize = 300;

CollisionGrid::CollisionGrid(int x, int y, int z) : x(x), y(y), z(z) {

    CollisionGrid();
}

CollisionGrid::CollisionGrid() {

    myObjects = vector<GameObject*>();
}

void CollisionGrid::AddObject(shared_ptr<GameObject> gameObject) {

    //cout << "Is Initialized: " << to_string(Initialized) << endl;
/*    cout << "Added "<< gameObject->Name <<" to grid: " << x << " " << y << " " << z << endl;*/

    if(gameObject->IgnoresAllCollisions)
        return;

    gameObject->GridIndex = make_tuple(x,y,z);
    myObjects.push_back(gameObject.get());
}

void CollisionGrid::RemoveObject(shared_ptr<GameObject> gameObject) {

    //cout << "Removing object " << gameObject->Name << "(" << gameObject->WorldID << ")";

    for (int i = 0; i < myObjects.size(); i++) {
        if (myObjects[i]->WorldID == gameObject->WorldID) {
            myObjects.erase(myObjects.begin() + i);
            break;
        }
    }
}
