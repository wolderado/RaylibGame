//
// Created by Salih on 07-Jan-24.
//

#include "Store.h"


tuple<int, int> GetAlignedTextPosition(string text,float xAlignment,float yAlignment, int fontSize) {
    int textWidth = MeasureText(text.c_str(), fontSize);
    int textHeight = fontSize;
    return make_tuple((float)textWidth * xAlignment,(float)textHeight * yAlignment);
}


void DrawTextPure(string text, int screenPosX, int screenPosY, int fontSize, Color textColor,int bckTextYOffset,float screenSizeRatio) {

    //Back Shadow Text
    Color bckColor = Utility::LerpColor(textColor,PALETTE_GRAY5,0.9f);
    DrawText(text.c_str(), screenPosX, screenPosY + bckTextYOffset, fontSize  * screenSizeRatio, bckColor);

    //Real Text
    DrawText(text.c_str(), screenPosX, screenPosY, fontSize * screenSizeRatio, textColor);
}

void WriteText(string text,float sizeMult,Color color,float xRatio,float yRatio,CanvasParams params,bool alignMiddle = true,bool alignRight = false)
{
    int titlePosX = params.panelStartX + params.panelWidth * xRatio;
    int titlePosY = params.panelStartY +  params.panelHeight * yRatio;

    if(alignMiddle) {
        auto [titleOffsetX, titleOffsetY] = GetAlignedTextPosition(text, 0.5f,0.5f,20 * params.screenSizeRatio * sizeMult);
        titlePosX -= titleOffsetX;
        titlePosY -= titleOffsetY;
    }
    else if(alignRight)
    {
        auto [titleOffsetX, titleOffsetY] = GetAlignedTextPosition(text,1.0f,0.5f, 20 * params.screenSizeRatio * sizeMult);
        titlePosX -= titleOffsetX;
        titlePosY -= titleOffsetY;
    }else
    {
        auto [titleOffsetX, titleOffsetY] = GetAlignedTextPosition(text,0.0f,0.5f, 20 * params.screenSizeRatio * sizeMult);
        titlePosX -= titleOffsetX;
        titlePosY -= titleOffsetY;
    }

    DrawTextPure(text,titlePosX,titlePosY,(float)20 * sizeMult,color,params.bckYOffset,params.screenSizeRatio);

}


void Store::RenderAndUpdate(float deltaTime,CanvasParams canvasParams) {



    storeTimer += deltaTime;
    anyItemBoughtAnimTimer+=deltaTime;

    //Anim

    //Open Anim
    if(storeTimer < StoreAnimDuration)
    {
        float t = storeTimer / StoreAnimDuration;
/*        storeAnimScale = Lerp(0.0f,1.0f,t);*/
        storeAnimScale = Utility::EaseOutElastic(t);
    }

    //Close Anim
    if(storeClosing)
    {
        float closeTimer = storeTimer - storeCloseTime;
        float t = closeTimer / StoreAnimDuration;
        storeAnimScale = 1.0f - Utility::EaseOutElastic(t);

        if(closeTimer > StoreAnimDuration)
        {
            //Disable store rendering
            IsStoreActive = false;
            isInputEnabled = true;
            OnStoreClose.Invoke();
        }
    }

    //Update params to animate open-close
    canvasParams.panelStartY = canvasParams.panelStartY + (canvasParams.panelHeight * 0.5f * (1.0f - storeAnimScale));
    canvasParams.panelHeight = canvasParams.panelHeight * storeAnimScale;

    //Background Fade
    Color fadeColor = Utility::GetColorWithCustomAlpha(PALETTE_GRAY5,150 * (Utility::Clamp01(storeAnimScale)));
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),fadeColor);

    //Panel
    Color fillColor = Utility::GetColorWithCustomAlpha(PALETTE_PURPLE2,150);
    DrawRectangle(canvasParams.panelStartX,canvasParams.panelStartY,canvasParams.panelWidth,canvasParams.panelHeight,fillColor);
    DrawRectangleLines(canvasParams.panelStartX,canvasParams.panelStartY,canvasParams.panelWidth,canvasParams.panelHeight,PALETTE_PURPLE1);




    if(storeAnimScale < 0.01f)
        return;

    //Title
    WriteText(STR_STORE_TITLE,1,PALETTE_WHITE,0.5f,0.1f,canvasParams);

    //Bottom text
    WriteText(STR_STORE_FOOTER,1,PALETTE_WHITE,0.5f,0.9f,canvasParams);

    //Key Press
    int keyPressed = GetKeyPressed();

    //Render items
    int playerScrap = Player::GetInstance()->GetScrap();
    int itemSize = sizeof(items) / sizeof(items[0]);
    for (int i = 0; i < itemSize; ++i) {

        //Color anim on item buy
        Color itemColor = PALETTE_WHITE;

        if(playerScrap < items[i].Price)
            itemColor = PALETTE_RED1;

        float sizeMult = 1.0f;

        //Buy animation
        if(items[i].BoughtAnimTime > 0.0f) {
            float t = Utility::Clamp01((storeTimer - items[i].BoughtAnimTime) / StoreAnimDuration);
            itemColor = Utility::LerpColor(PALETTE_YELLOW1, itemColor,t);
            sizeMult = 1.0f + ((1.0f - t) * 0.25f);

            if(t > 1.0f)
                items[i].BoughtAnimTime = 0.0f;
        }

        //Item Display text
        int displayIndex = i+1;
        stringstream displayText;
        displayText << "[" << displayIndex << "]. " << (items[i].Text);
        WriteText(displayText.str(),1.0f,itemColor,0.05f,0.3f + i * 0.1f,canvasParams,false);

        //Cost & Level text
        stringstream right;
        right << "(" << items[i].Price << " " << STR_STORE_ITEM_SCRAP_NAME << ") ";
        if(items[i].LevelType)
            right << STR_STORE_ITEM_LV << items[i].Level;
        else
            right << items[i].Level;
        WriteText(right.str(),sizeMult,itemColor,0.95f,0.3f + i * 0.1f,canvasParams,false,true);


        //Buy action
        if(keyPressed - KEY_ONE == i)
        {
            BuyItem(i);
        }
    }

    //Front flash on item buy
    if(anyItemBoughtAnimTimer < StoreItemBuyFlashDuration) {
        float t = Utility::Clamp01(anyItemBoughtAnimTimer / StoreItemBuyFlashDuration);

        Color fadeColor = Utility::GetColorWithCustomAlpha(PALETTE_PURPLE1,(float)100 * (1.0f - t));
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),fadeColor);
    }

}

void Store::BuyItem(int itemIndex) {

    float playerScrap = Player::GetInstance()->GetScrap();

    if(playerScrap < items[itemIndex].Price)
        return;

    Player::GetInstance()->AddScrap(-items[itemIndex].Price);

    if(itemIndex > 0)
        items[itemIndex].Level++;

    items[itemIndex].BoughtAnimTime = storeTimer;
    anyItemBoughtAnimTimer = 0;

    ProcessItemEffect(itemIndex);

    OnBuyItem.Invoke(itemIndex);
}

void Store::CloseStore() {
    storeClosing = true;
    storeCloseTime = storeTimer;
    isInputEnabled = false;
}

void Store::OpenStore() {
    storeClosing = false;
    storeTimer = 0;
    IsStoreActive = true;
    isInputEnabled = true;
    anyItemBoughtAnimTimer = 999;

    int itemSize = sizeof(items) / sizeof(items[0]);
    for (int i = 0; i < itemSize; ++i) {
        items[i].BoughtAnimTime = 0;
    }
}

Store::Store() {

}



void Store::ProcessItemEffect(int itemIndex) {

    switch (itemIndex) {
        case 0:
            //Heal player
            Player::GetInstance()->Heal(Player::GetInstance()->GetMaxHealth() * 0.25f);
            break;
        case 1:
            //Increase ally fighters
            BattleManager::GetInstance()->AllyFighterBought++;
            break;
        default:
            //Upgrade player stat
            Player::GetInstance()->UpgradeStat(items[itemIndex].UpgradeType);
    }
}
