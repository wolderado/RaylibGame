//
// Created by Salih on 03-Jan-24.
//

#include "BattleManager.h"

void BattleManager::Init() {
    worldInstance = World::GetInstance();
}

void BattleManager::UpdateAI(float deltaTime) {

    if(worldInstance == nullptr) {
        cout << "World instance is null!" << endl;
        return;
    }


    for (auto& pair : worldInstance->activeGameObjects) {

        if(pair.second->GetHealth() > 0 && pair.second->HasTag("Fighter"))
        {
            ProcessAI(dynamic_pointer_cast<Fighter>(pair.second));
        }
    }
}

void BattleManager::ProcessAI(shared_ptr<Fighter> fighter) {

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

shared_ptr<GameObject> BattleManager::GetRandomShip(TEAM myTeam) {
    return shared_ptr<GameObject>();
}
