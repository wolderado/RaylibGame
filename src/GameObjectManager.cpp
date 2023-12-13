//
// Created by Salih on 12-Dec-23.
//

#include "GameObjectManager.h"



void GameObjectManager::UpdateAll(float deltaTime) {
    for (shared_ptr<GameObject> &gameObject : activeGameObjects) {
        //cout << "Updating game object" << endl;
        gameObject->Update(deltaTime);
        gameObject->Render(deltaTime);
    }
}

shared_ptr<GameObject> GameObjectManager::CreateNewObject() {
    shared_ptr<GameObject> gameObject = make_shared<GameObject>();
    InitObject(gameObject);
    return gameObject;
}

shared_ptr<GameObject> GameObjectManager::CreateNewAsteroid(Vector3 position) {
    shared_ptr<Asteroid> newAsteroid = make_shared<Asteroid>();
    InitObject(newAsteroid);

    newAsteroid->SetTeam(TEAM_NEUTRAL);

/*    //Load models
    Mesh sphereMesh = GenMeshSphere(1.0f, 8, 8);

    // Mesh vertices position array
    for (int i = 0; i < sphereMesh.vertexCount; i++)
    {
        if(i%3 == 0)
            continue;

        Vector3 newVertex = { sphereMesh.vertices[3*i], sphereMesh.vertices[3*i + 1], sphereMesh.vertices[3*i + 2] };
        Vector3 normal = { sphereMesh.normals[3*i], sphereMesh.normals[3*i + 1], sphereMesh.normals[3*i + 2] };


        newVertex = Vector3Add(newVertex, Vector3Scale(normal,GetRandomValue(-100,100)  * 0.001f));

        sphereMesh.vertices[3*i] = newVertex.x;
        sphereMesh.vertices[3*i + 1] = newVertex.y;
        sphereMesh.vertices[3*i + 2] = newVertex.z;
    }


    UpdateMeshBuffer(sphereMesh, 0, sphereMesh.vertices, sphereMesh.vertexCount * 3 * sizeof(float),0);
    TestModel = LoadModelFromMesh(sphereMesh);
    newObject->SetModel(LoadModelFromMesh(sphereMesh));*/


    string modelPath = "resources/Asteroid" + to_string(GetRandomValue(1, 3)) + ".glb";
    Model asteroidModel = LoadModel(modelPath.c_str());
    newAsteroid->SetModel(asteroidModel);


    newAsteroid->Position = position;
    newAsteroid->Scale = Vector3Scale(Vector3One(), GetRandomValue(1, 50) * 0.1f);
    newAsteroid->Rotation.x = GetRandomValue(0, 360);
    newAsteroid->Rotation.y = GetRandomValue(0, 360);
    newAsteroid->Rotation.z = GetRandomValue(0, 360);



    return newAsteroid;
}

void GameObjectManager::InitObject(shared_ptr<GameObject> target) {
    activeGameObjects.push_back(target);
}
