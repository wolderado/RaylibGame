//
// Created by Salih on 07-Jan-24.
//

#ifndef SRC_STORE_H
#define SRC_STORE_H

#include <string>
#include <sstream>
#include "Player.h"
#include "Globals.h"
#include "Event.h"
#include "BattleManager.h"


struct StoreItem
{
    string Text;
    int Price;
    int Level;
    PLAYER_UPGRADE_TYPE UpgradeType;
    bool LevelType = true;


    float BoughtAnimTime;
};

struct CanvasParams
{
    int panelStartX;
    int panelStartY;
    int panelWidth;
    int panelHeight;
    float screenSizeRatio;
    int bckYOffset;
};

class Store {
public:
    const float StoreAnimDuration = 0.35f;
    const float StoreItemBuyAnimDuration = 0.25f;
    const float StoreItemBuyFlashDuration = 0.5f;

    Store();
    void RenderAndUpdate(float deltaTime,CanvasParams canvasParams);
    void OpenStore();
    void CloseStore();
    float GetStoreTimer() { return storeTimer; };
    bool IsInputEnabled() { return isInputEnabled; };
    float GetStoreAnimScale() { return storeAnimScale; };
    float GetBuyAnimTimer() { return anyItemBoughtAnimTimer; };
    Event<void> OnStoreClose;
    Event<int> OnBuyItem;

    bool IsStoreActive = false;


private:

    //STRING KEYS for easy localization for the future
    //TODO: String tags for value changing
    string STR_ITEM_REPAIR = "Repair %25 Health";
    string STR_ITEM_ALLYSHIP = "Buy Ally Ship x1";
    string STR_ITEM_UPGRADE_DAMAGE = "Upgrade: +%15 Damage";
    string STR_ITEM_UPGRADE_SPEED = "Upgrade: +%25 Thrust";
    string STR_ITEM_UPGRADE_MAXHEALTH = "Upgrade: +%25 Health";
    string STR_STORE_TITLE = "Buy Items by Pressing Number Keys";
    string STR_STORE_FOOTER = "Press B to Close";
    string STR_STORE_ITEM_SCRAP_NAME = "scrap";
    string STR_STORE_ITEM_LV = "Lv.";


    StoreItem items[5] = {
            {STR_ITEM_REPAIR,100, 1, PLAYER_UPGRADE_TYPE::None,false},
            {STR_ITEM_ALLYSHIP,200 , 0, PLAYER_UPGRADE_TYPE::None,false},
            {STR_ITEM_UPGRADE_DAMAGE,500, 1,PLAYER_UPGRADE_TYPE::Damage},
            {STR_ITEM_UPGRADE_SPEED,500, 1,PLAYER_UPGRADE_TYPE::Speed},
            {STR_ITEM_UPGRADE_MAXHEALTH,500,1,PLAYER_UPGRADE_TYPE::Health}
    };

    float storeTimer = 0;
    float storeCloseTime = 0;
    float storeAnimScale = 0;
    bool storeClosing = false;
    bool isInputEnabled = true;
    float anyItemBoughtAnimTimer = 0;



    void BuyItem(int itemIndex);
    void ProcessItemEffect(int itemIndex);
};


#endif //SRC_STORE_H
