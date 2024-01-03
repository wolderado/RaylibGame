//
// Created by Salih on 12-Dec-23.
//

#include "World.h"

World* World::instance = nullptr;

World::World() {

}


World* World::GetInstance() {
    if(instance == nullptr)
        instance = new World();

    return instance;
}


void World::UpdateAll(float deltaTime) {

    for (auto& pair : activeGameObjects) {
/*        cout << "Updating game object " << pair.second->Name << endl;*/
        pair.second->Update(deltaTime);
        CheckCollision(pair.second.get());
        pair.second->Render(deltaTime);
        pair.second->LateUpdate(deltaTime);

        //Cleanup objects
        if(pair.second->GetHealth() <= 0)
        {
            pair.second->Destroy();
            OnGameObjectDestroyed(pair.second);
            activeGameObjects.erase(pair.first);
        }
    }
}

shared_ptr<GameObject> World::CreateNewObject() {
    shared_ptr<GameObject> gameObject = make_shared<GameObject>();
;
    InitObject(gameObject);
    return gameObject;
}

shared_ptr<GameObject> World::CreateNewAsteroid(Vector3 position,float maxSize) {


    float asteroidSize = GetRandomValue(25,maxSize);

    shared_ptr<Asteroid> newAsteroid = make_shared<Asteroid>(asteroidSize);


    newAsteroid->SetTeam(TEAM_NEUTRAL);
    newAsteroid->Name = "Asteroid";
    newAsteroid->AddTag("Asteroid");

    string modelPath = "resources/Asteroid" + to_string(GetRandomValue(1, 3)) + ".glb";
    Model asteroidModel = LoadModel(modelPath.c_str());
    newAsteroid->SetModel(asteroidModel);



    newAsteroid->Position = position;
    newAsteroid->Scale = Vector3Scale(Vector3One(), asteroidSize * 0.1f);
    newAsteroid->Rotation.x = GetRandomValue(0, 360);
    newAsteroid->Rotation.y = GetRandomValue(0, 360);
    newAsteroid->Rotation.z = GetRandomValue(0, 360);
    newAsteroid->Mass = asteroidSize;
    InitObject(newAsteroid);

    newAsteroid->SetHealth(asteroidSize);

    return shared_ptr<GameObject>(newAsteroid);
}

void World::InitObject(shared_ptr<GameObject> target) {
    //Get the biggest scale value to approximate collision size
    target->CollisionSize *= fmax(fmax(target->Scale.x,target->Scale.y),target->Scale.z);
    activeGameObjects[nextObjectId] = target;
    target->WorldID = nextObjectId;
    target->SetVelocity(Vector3Zero());
    target->SetHealth(100);
    target->Name = target->Name + string(" [ID=") + to_string(nextObjectId) + string("]");
    target->OnInit();
    nextObjectId++;

    if(OnNewObjectCreated != nullptr)
        OnNewObjectCreated(target);
}

void World::CheckCollision(GameObject* object) {

    //TODO: Optimize this function further by using Spatial partitioning or grid based collision detection

    if (object->CanCollide == false)
        return;

    if (Vector3LengthSqr(object->GetVelocity()) < 0.001f)
        return;

    for (const auto &gmPair: activeGameObjects) {
        if (object->CanCollide == false || gmPair.second.get() == object)
            continue;

        if (CheckCollisionPure(object->Position, object->CollisionSize, gmPair.second->Position,
                               gmPair.second->CollisionSize)) {
            Vector3 totalVel = Vector3Add(object->GetVelocity(), gmPair.second->GetVelocity());
            object->OnCollision(gmPair.second.get(), totalVel);
            gmPair.second->OnCollision(object, totalVel);
            break;
        }
    }
}

shared_ptr<GameObject> World::CheckBulletCollision(Vector3 bulletPosition) {
    //TODO: Optimize this function further by using Spatial partitioning or grid based collision detection

    for (const auto &gmPair: activeGameObjects) {
        if (CheckCollisionPure(bulletPosition, 0.1f, gmPair.second->Position, gmPair.second->CollisionSize)) {
            return activeGameObjects[gmPair.first];
        }
    }

    return nullptr;
}

void World::OnGameObjectDestroyed(shared_ptr<GameObject>& destroyedObject) {

    if(destroyedObject->HasTag("Asteroid"))
    {
        Asteroid* destroyedAsteroid = dynamic_cast<Asteroid*>(destroyedObject.get());

        if(destroyedAsteroid->AsteroidSize < 25)
            return;

        //cout << destroyedAsteroid->AsteroidSize << " sized asteroid destroyed, creating 2 new ones" << endl;


        float newAsteroidSize = destroyedAsteroid->AsteroidSize/2.0f;
        int minForceMagnitude = 60;
        int maxForceMagnitude = 60;
        float finalForce = GetRandomValue(minForceMagnitude,maxForceMagnitude) * 0.01f;

        Vector3 rndDirection = Utility::GetRandomDirection();

        for (int i = 0; i < 2; ++i) {
            shared_ptr<GameObject> asteroid = World::GetInstance()->CreateNewAsteroid(destroyedAsteroid->Position,newAsteroidSize);
            asteroid->SetVelocity(Vector3Scale(rndDirection, finalForce));
        }
    }

}

bool World::CheckCollisionPure(Vector3 position1, float size1, Vector3 position2, float size2) {
    return CheckCollisionSpheres(position1, size1, position2,size2);
}

shared_ptr<GameObject> World::CreateNewFighter(TEAM team, Vector3 position) {
    shared_ptr<Fighter> newFighter = make_shared<Fighter>(team,position);
    newFighter->SetTeam(team);
    newFighter->Name = "Fighter";
    newFighter->AddTag("Fighter");

    string modelPath = "resources/Ship.glb";
    Model fighterModel = LoadModel(modelPath.c_str());
    newFighter->SetModel(fighterModel);

    newFighter->Position = position;
    newFighter->Scale = Vector3Scale(Vector3One(), 0.5f);
/*    newFighter->Rotation.x = GetRandomValue(0, 360);
    newFighter->Rotation.y = GetRandomValue(0, 360);
    newFighter->Rotation.z = GetRandomValue(0, 360);*/
    newFighter->Mass = 1;
    newFighter->CollisionSize = 2;
    newFighter->SetVelocity(Utility::GetRandomDirection());
    newFighter->SetHealth(100);

    InitObject(newFighter);

    return shared_ptr<GameObject>(newFighter);
}

