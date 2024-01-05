//
// Created by Salih on 03-Jan-24.
//

#ifndef SRC_BATTLEMANAGER_H
#define SRC_BATTLEMANAGER_H
#include <iostream>

#include "World.h"



enum BattleState
{
    Waiting,
    BattleStarted
};

class BattleManager {
public:
    static BattleManager* GetInstance();
    void Init();
    void UpdateAI(float deltaTime);
    void ProcessAI(shared_ptr<Fighter> object);
    BattleState GetBattleState(){ return currentBattleState; }
    float GetWaitTimer(){ return waitTimer; }


private:
    World* worldInstance;
    static BattleManager* instance;
    BattleState currentBattleState = BattleState::Waiting;
    float waitTimer = 0;

    void ProcessWait(float deltaTime);
    void ProcessBattle(float deltaTime);
    void StartBattle();
    void EndBattle();
};


#endif //SRC_BATTLEMANAGER_H
