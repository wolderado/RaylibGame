//
// Created by Salih on 12-Dec-23.
//

#include "World.h"

World* World::instance = nullptr;

World::World() {
    instance = this;
}


World* World::GetInstance() {
    if(instance == nullptr)
        instance = new World();

    return instance;
}


void World::UpdateAll(float deltaTime) {
    for (shared_ptr<GameObject> &gameObject : activeGameObjects) {
        //cout << "Updating game object" << endl;
        gameObject->Update(deltaTime);
        CheckCollision(gameObject.get());
        gameObject->Render(deltaTime);
    }
}

shared_ptr<GameObject> World::CreateNewObject() {
    shared_ptr<GameObject> gameObject = make_shared<GameObject>();
    InitObject(gameObject);
    return gameObject;
}

shared_ptr<GameObject> World::CreateNewAsteroid(Vector3 position) {
    shared_ptr<Asteroid> newAsteroid = make_shared<Asteroid>();


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

    float asteroidSize = GetRandomValue(1, 50) * 0.1f;

    newAsteroid->Position = position;
    newAsteroid->Scale = Vector3Scale(Vector3One(), asteroidSize);
    newAsteroid->Rotation.x = GetRandomValue(0, 360);
    newAsteroid->Rotation.y = GetRandomValue(0, 360);
    newAsteroid->Rotation.z = GetRandomValue(0, 360);
    newAsteroid->Mass = asteroidSize;

    InitObject(newAsteroid);

    return newAsteroid;
}

void World::InitObject(shared_ptr<GameObject> target) {
    //Get the biggest scale value to approximate collision size
    target->CollisionSize *= fmax(fmax(target->Scale.x,target->Scale.y),target->Scale.z);
    activeGameObjects.push_back(target);
}

shared_ptr<Player> World::CreatePlayer() {
    player.Init();
    shared_ptr<Player> newPlayer = make_shared<Player>(player);
    InitObject(newPlayer);
    return newPlayer;
}

void World::CheckCollision(GameObject* object) {

    if(object->CanCollide == false)
        return;

    if(Vector3LengthSqr(object->GetVelocity()) < 0.001f)
        return;

    for (shared_ptr<GameObject> &otherObject : activeGameObjects)
    {
        if(object->CanCollide == false || otherObject.get() == object)
            continue;

        if(CheckCollisionSpheres(object->Position,object->CollisionSize,otherObject->Position,otherObject->CollisionSize))
        {
            Vector3 totalVel = Vector3Add(object->GetVelocity(),otherObject->GetVelocity());
            object->OnCollision(otherObject.get(),totalVel);
            otherObject->OnCollision(object,totalVel);
            break;
        }
    }
}
