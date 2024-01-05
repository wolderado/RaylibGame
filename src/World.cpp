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


void World::DebugHurtClosestObject()
{
    if(IsKeyPressed(KEY_P))
    {
        Vector3 playerPos = Player::GetInstance()->Position;
        float closestDist = 999999;
        GameObject* closestObject = nullptr;
        for (auto const& x : activeGameObjects)
        {
            if(x.second->GetTeam() == TEAM_PLAYER)
                continue;

            float dist = Vector3Distance(playerPos,x.second->Position);
            if(dist < closestDist)
            {
                closestDist = dist;
                closestObject = x.second.get();
            }
        }

        if(closestObject != nullptr)
            closestObject->Hurt(99999);
    }

}


float debugPrintTimer = 0;
void World::UpdateAll(float deltaTime) {


    DebugHurtClosestObject();

    debugPrintTimer += deltaTime;
    for (auto gameObjectIterator = activeGameObjects.begin(); gameObjectIterator != activeGameObjects.end();) {
/*        cout << "Updating game object " << gameObjectIterator.second->Name << endl;*/

        if(gameObjectIterator->second->IsEnabled() == false) {
            ++gameObjectIterator;
            continue;
        }


        gameObjectIterator->second->Update(deltaTime);
        UpdateGridForObject(shared_ptr<GameObject>(gameObjectIterator->second));
        CheckCollision(gameObjectIterator->second.get());
        gameObjectIterator->second->Render(deltaTime);
        gameObjectIterator->second->LateUpdate(deltaTime);


        //Cleanup objects
        if(gameObjectIterator->second->GetHealth() <= 0)
        {
            gameObjectIterator->second->Destroy();
            OnGameObjectDestroyed(gameObjectIterator->second);
            auto eraseIterator = gameObjectIterator;
            ++gameObjectIterator;
            activeGameObjects.erase(eraseIterator);
        }else
            ++gameObjectIterator;


        if(debugPrintTimer > 1)
        {
            debugPrintTimer = 0;
            cout << "DEBUG: Object Count: "    << activeGameObjects.size()
            << " Ally Fighters: "        << AllyFighterCount
            << " Enemy Fighters: "      << EnemyFighterCount
            << " GridCount:"            << Grid.size()
            << " BulletCount: "         << DEBUG_BulletCount << endl;
        }
    }
}

void World::GenerateWorld() {

    Grid = map<tuple<int,int,int>,CollisionGrid>();


    //Generate Asteroids
    for (int i = 0; i < 100; ++i) {
        Vector3 rndPos = Utility::GetRandomPosInsideMap();
        CreateNewAsteroid(rndPos);
    }

    //Generate Enemy ships
    for (int i = 0; i < 0; ++i) {
        Vector3 rndPos = Utility::GetRandomPosInsideMap();
        CreateNewFighter(TEAM_ENEMY,rndPos);
    }

    //Genereate Ally ships
    for (int i = 0; i < 0; ++i) {
        Vector3 rndPos = Utility::GetRandomPosInsideMap();
        CreateNewFighter(TEAM_ALLY,rndPos);
    }

    IsWorldReady = true;

}



#pragma region Object Creation/Deletion

shared_ptr<GameObject> World::CreateNewObject() {
    shared_ptr<GameObject> gameObject = make_shared<GameObject>();

    InitObject(gameObject);
    return gameObject;
}


void World::InitObject(shared_ptr<GameObject> target) {
    //Get the biggest scale value to approximate collision size
    target->CollisionSize *= fmax(fmax(target->Scale.x,target->Scale.y),target->Scale.z);
    activeGameObjects[nextObjectId] = target;
    target->WorldID = nextObjectId;
    target->SetVelocity(Vector3Zero());
    target->Name = target->Name + string(" [ID=") + to_string(nextObjectId) + string("]");
    target->OnInit();
    nextObjectId++;

    //Save To Grid
    if(target->IgnoresAllCollisions == false) {
        auto [xPos, yPos, zPos] = GetGridPosFromRealPos(target->Position);
        CollisionGrid *newGrid = GetGrid(xPos, yPos, zPos);
        newGrid->AddObject(shared_ptr<GameObject>(target));
    }

    //cout << "Total Grids: " << Grid.size() << " Grid Objects " << newGrid->myObjects.size() << endl;

    if(OnNewObjectCreated != nullptr)
        OnNewObjectCreated(target);
}




shared_ptr<GameObject> World::CreateNewFighter(TEAM team, Vector3 position) {

    shared_ptr<Fighter> newFighter = make_shared<Fighter>(team,position);
    newFighter->SetTeam(team);
    newFighter->AddTag("Fighter");

    string modelPath = "resources/Ship.glb";
    Model fighterModel = LoadModel(modelPath.c_str());
    newFighter->SetModel(fighterModel);

    newFighter->Position = position;
    newFighter->Scale = Vector3Scale(Vector3One(), 0.5f);
    newFighter->Mass = 1;
    newFighter->CollisionSize = 2;
    newFighter->SetVelocity(Utility::GetRandomDirection());
    newFighter->SetHealth(STAT_HEALTH_FIGHTER);

    InitObject(newFighter);


    if(team == TEAM_ALLY)
        AllyFighterCount++;
    else
        EnemyFighterCount++;

    return shared_ptr<GameObject>(newFighter);
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

void World::OnGameObjectDestroyed(shared_ptr<GameObject>& destroyedObject) {

    if(destroyedObject->HasTag("Asteroid"))
    {
        Asteroid* destroyedAsteroid = dynamic_cast<Asteroid*>(destroyedObject.get());

        if(destroyedAsteroid->AsteroidSize < 25)
            return;

        //cout << destroyedAsteroid->AsteroidSize << " sized asteroid destroyed, creating 2 new ones" << endl;


        float newAsteroidSize = destroyedAsteroid->AsteroidSize/2.0f;
        int minForceMagnitude = 5;
        int maxForceMagnitude = 10;
        float finalForce = GetRandomValue(minForceMagnitude,maxForceMagnitude) * 0.01f;

        Vector3 rndDirection = Utility::GetRandomDirection();

        shared_ptr<GameObject> asteroid = World::GetInstance()->CreateNewAsteroid(destroyedAsteroid->Position,newAsteroidSize);
        asteroid->SetVelocity(Vector3Scale(rndDirection, finalForce));

        shared_ptr<GameObject> asteroid2 = World::GetInstance()->CreateNewAsteroid(destroyedAsteroid->Position,newAsteroidSize);
        asteroid2->SetVelocity(Vector3Scale(Vector3Scale(rndDirection,-1), finalForce));

    }

    if(destroyedObject->HasTag("Fighter"))
    {
        //cout << "Fighter destroyed" << endl;

        if(destroyedObject->GetTeam() == TEAM_ALLY)
            AllyFighterCount--;
        else
            EnemyFighterCount--;
    }

    //Update grid


    CollisionGrid* grid = GetGrid(get<0>(destroyedObject->GridIndex),get<1>(destroyedObject->GridIndex),get<2>(destroyedObject->GridIndex));
    grid->RemoveObject(destroyedObject);
}


#pragma endregion

#pragma region Collision
void World::CheckCollision(GameObject* object) {

    if(object == nullptr)
        return;

    if (object->CanCollide == false || object->IgnoresAllCollisions)
        return;

    if (Vector3LengthSqr(object->GetVelocity()) < 0.001f)
        return;

    CollisionGrid* objectGrid = GetGridFromRealPos(object->Position);

    for (int i = 0; i < objectGrid->myObjects.size(); ++i) {

        GameObject* otherObject = objectGrid->myObjects[i];

        if (object->CanCollide == false || otherObject == object)
            continue;



        if (CheckCollisionSingle(object->Position, object->CollisionSize, otherObject->Position,otherObject->CollisionSize)) {

            Vector3 totalVel = Vector3Add(object->GetVelocity(), otherObject->GetVelocity());
            object->OnCollision(otherObject, totalVel);

            if(otherObject == nullptr)
                cout << "NULL POINTER!!" << endl;

            otherObject->OnCollision(object, totalVel);
            break;
        }
    }
}

tuple<bool,GameObject*> World::CheckBulletCollision(Vector3 bulletPosition,TEAM bulletTeam) {

    CollisionGrid* bulletGrid = GetGridFromRealPos(bulletPosition);
    //vector<GameObject*> objectsToScan = GetNearbyObjects(bulletPosition);

    //DrawLine3D(bulletPosition, ConvertGridPosToRealPos(make_tuple(bulletGrid->x,bulletGrid->y,bulletGrid->z)), PALETTE_GREEN1);

    for (const auto &target: bulletGrid->myObjects) {

        //DrawLine3D(bulletPosition,  target->Position, PALETTE_RED1);
        //DrawSphereWires(gmPair->Position,50,6,6,PALETTE_GREEN1);

        if(target->GetTeam() == bulletTeam || target->IgnoresAllCollisions)
            continue;

        if(target->GetTeam() == TEAM_PLAYER && bulletTeam == TEAM_ALLY)
            continue;

        if (CheckCollisionSingle(bulletPosition, 0.1f, target->Position, target->CollisionSize)) {
            return make_tuple(true,target);
        }
    }


    return make_tuple(false, nullptr);
}


bool World::CheckCollisionSingle(Vector3 position1, float size1, Vector3 position2, float size2) {
    return CheckCollisionSpheres(position1, size1, position2,size2);
}



void World::UpdateGridForObject(shared_ptr<GameObject> object) {

    //DrawLine3D(object->Position,ConvertGridPosToRealPos(object->GridIndex),PALETTE_GREEN1);

    if(Vector3LengthSqr(object->GetVelocity()) < 0.01f)
        return;

    if(object->IgnoresAllCollisions)
        return;

    if(Utility::IsInsideMapArea(object->Position) == false)
        return;

    Vector3 realPos = object->Position;
    auto [xPos, yPos,zPos] = GetGridPosFromRealPos(realPos);

    //cout << "Updating grid for " << object->Name << " to " << xPos << " " << yPos << " " << zPos << endl;

    if(object->GridIndex == make_tuple(xPos,yPos,zPos))
        return;

    CollisionGrid* oldGrid = GetGrid(get<0>(object->GridIndex),get<1>(object->GridIndex),get<2>(object->GridIndex));
    oldGrid->RemoveObject(object);

    CollisionGrid* newGrid = GetGrid(xPos,yPos,zPos);
    newGrid->AddObject(object);
}



CollisionGrid* World::GetGrid(int x, int y, int z) {

    //cout << "Exist Check: " << to_string(Grid.count(make_tuple(x,y,z))) << " for " << x << y << z << endl;

    if(Grid.count(make_tuple(x,y,z)) == 0) {
        //cout << "Creating a new grid for" << x << " " << y << " " << z << endl;

        //Create new grid
        Grid[make_tuple(x,y,z)] = CollisionGrid(x,y,z);
        Grid[make_tuple(x,y,z)].Initialized = true;
    }

    return &Grid[make_tuple(x,y,z)];
}

Vector3 World::ConvertGridPosToRealPos(std::tuple<int, int, int> gridIndex) {

    float realPosX = get<0>(gridIndex) * CollisionGrid::CellSize;
    float realPosY = get<1>(gridIndex) * CollisionGrid::CellSize;
    float realPosZ = get<2>(gridIndex) * CollisionGrid::CellSize;
    return (Vector3){realPosX,realPosY,realPosZ};
}

CollisionGrid* World::GetGridFromRealPos(Vector3 realPos) {

    auto [x, y,z] = GetGridPosFromRealPos(realPos);
    return GetGrid(x,y,z);
}


tuple<int, int, int> World::GetGridPosFromRealPos(Vector3 realPos) {
    int xPos = (int)(realPos.x / CollisionGrid::CellSize);
    int yPos = (int)(realPos.y / CollisionGrid::CellSize);
    int zPos = (int)(realPos.z / CollisionGrid::CellSize);
    return make_tuple(xPos,yPos,zPos);
}

#pragma endregion


