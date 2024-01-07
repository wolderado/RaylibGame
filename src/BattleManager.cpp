//
// Created by Salih on 03-Jan-24.
//

#include "BattleManager.h"

BattleManager* BattleManager::instance = nullptr;

BattleManager* BattleManager::GetInstance() {
    if(instance == nullptr)
        instance = new BattleManager();

    return instance;
}

void BattleManager::Init() {
    worldInstance = World::GetInstance();

    waitTimer = GAME_SHOP_WAIT_TIME;
}

void BattleManager::UpdateAI(float deltaTime) {

    if(worldInstance == nullptr) {
        cout << "World instance is null!" << endl;
        return;
    }

    if(worldInstance->IsWorldStopped())
        return;

    if(currentBattleState == BattleState::Waiting) {
        ProcessWait(deltaTime);
    }
    else if(currentBattleState == BattleState::BattleStarted) {
        ProcessBattle(deltaTime);
    }
}


void BattleManager::ProcessWait(float deltaTime) {


    if(waitTimerActive)
        waitTimer -= deltaTime;

    if(waitTimer < 0) {
        StartBattle();
        waitTimer = 0;
    }
}


void BattleManager::ProcessBattle(float deltaTime) {

    //End the battle and start shop timer
    if(worldInstance->EnemyFighterCount <= 0) {
        EndBattle();
        return;
    }


    //Update AI
    for (auto& pair : worldInstance->activeGameObjects) {

        if(pair.second->GetHealth() > 0 && pair.second->HasTag("Fighter"))
        {
            ProcessAI(dynamic_pointer_cast<Fighter>(pair.second));
        }
    }
}



void BattleManager::ProcessAI(shared_ptr<Fighter> fighter) {

    //Process FSM here because we need to access the world objects
    if(fighter->CurrentAIState == AIState::FindTarget)
    {
        //%50 chance to target the nearest enemy for semi-arcade behaviour
        if(GetRandomValue(0,100) < 50) {

            //Search for closest target
            float closestDist = 99999999;
            shared_ptr<GameObject> closestTarget = nullptr;
            for (auto &pair: worldInstance->activeGameObjects) {
                if (pair.second->GetHealth() <= 0 || pair.second->WorldID == fighter->WorldID)
                    continue;

                bool targetTeam = false;
                if (fighter->GetTeam() == TEAM_ENEMY &&
                    (pair.second->GetTeam() == TEAM_PLAYER || pair.second->GetTeam() == TEAM_ALLY))
                    targetTeam = true;

                if (fighter->GetTeam() == TEAM_ALLY && pair.second->GetTeam() == TEAM_ENEMY)
                    targetTeam = true;

                if (targetTeam == false)
                    continue;

                float dist = Vector3Distance(fighter->Position, pair.second->Position);
                if (dist < closestDist) {
                    closestDist = dist;
                    closestTarget = pair.second;
                }
            }

            if (closestTarget != nullptr) {
                fighter->SetTarget(closestTarget);
            }
        }else {

            //Select a target randomly (More exciting)
            int randomIndex = GetRandomValue(0, worldInstance->activeGameObjects.size() - 1);
            auto it = std::next(worldInstance->activeGameObjects.begin(), randomIndex);

            shared_ptr<GameObject> rndObject = it->second;
            if (rndObject->GetHealth() > 0 && rndObject->WorldID != fighter->WorldID) {
                bool targetTeam = false;
                if (fighter->GetTeam() == TEAM_ENEMY &&
                    (rndObject->GetTeam() == TEAM_PLAYER || rndObject->GetTeam() == TEAM_ALLY))
                    targetTeam = true;

                if (fighter->GetTeam() == TEAM_ALLY && rndObject->GetTeam() == TEAM_ENEMY)
                    targetTeam = true;

                if (targetTeam == false)
                    return;

/*                if (rndObject->GetTeam() == TEAM_PLAYER)
                    cout << "Fighter found player!" << endl;*/

                fighter->SetTarget(rndObject);
            }
        }

    }
}

void BattleManager::StartBattle() {
    currentBattleState = BattleState::BattleStarted;

    worldInstance->DEBUG_DestroyedObjectCount = 0;

    //Generate Enemy ships
    int enemyCount = 5 + (EnemyCountIncreasePerWave * (currentWave-1) );
    float enemyHealth = EnemyHealthIncreasePerWave * (float)STAT_HEALTH_FIGHTER;

    for (int i = 0; i < enemyCount; ++i) {
        Vector3 rndPos = Utility::GetRandomPosInsideMap();
        shared_ptr<GameObject> fighter = worldInstance->CreateNewFighter(TEAM_ENEMY,rndPos);
        fighter->SetHealth(enemyHealth);
    }

/*    cout << "Wave started! Enemy count: " << enemyCount << " Enemy health: " << enemyHealth << endl;*/

    //Generate Ally ships
    for (int i = 0; i < AllyFighterBought; ++i) {
        Vector3 rndPos = Utility::GetRandomPosInsideMap();
        worldInstance->CreateNewFighter(TEAM_ALLY,rndPos);
    }

    //Generate Asteroids to fill empty spaces
    int asteroidsMissing = MAP_ASTEROID_COUNT - worldInstance->AsteroidCount;
    for (int i = 0; i < asteroidsMissing; ++i) {
        Vector3 rndPos = Utility::GetRandomPosInsideMap();
        worldInstance->CreateNewAsteroid(rndPos);
    }
}

void BattleManager::EndBattle() {
    currentBattleState = BattleState::Waiting;
    waitTimer = GAME_SHOP_WAIT_TIME;
    currentWave++;

    //Destroy all ally ships
    for (auto& pair : worldInstance->activeGameObjects) {
        if(pair.second->GetHealth() > 0 && pair.second->HasTag("Fighter") && pair.second->GetTeam() == TEAM_ALLY)
        {
            pair.second->Hurt(99999);
        }
    }
}
