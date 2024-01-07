//
// Created by Salih on 16-Dec-23.
//

#include "HUD.h"

void HUD::Init(shared_ptr<Player> targetPlayer)
{
    player = shared_ptr<Player>(targetPlayer);
    playerGunModel = LoadModel("resources/PlayerGun.glb");
    playerGunMuzzleModel = LoadModel("resources/PlayerGunMuzzle.glb");
    playerShipModel = LoadModel("resources/Ship.glb");


    player->OnShoot.AddListener(std::bind(&HUD::GunShoot, this, std::placeholders::_1));
    player->OnScrapGain.AddListener(std::bind(&HUD::PlayScrapGainAnim, this, std::placeholders::_1));
    player->OnPlayerDeath.AddListener(std::bind(&HUD::PlayerDied, this));


    store.OnStoreClose.AddListener(std::bind(&HUD::StoreCloseAnimFinished, this));
    store.OnBuyItem.AddListener(std::bind(&HUD::PlayerBuyAnim, this, std::placeholders::_1));

    currentTutorialStage = TutorialStages::StageBasics;
    BattleManager::GetInstance()->SetWaitTimerState(false);
    fadeInTimer = 0;
}


void HUD::Render(float deltaTime)
{
    //TODO: Implement a canvas item based with render order and parent-child capabilities

    screenSizeRatio = (float)GetScreenWidth() / (float)DefaultScreenWidth;


    //Wave Timer, Enemy count, Wave count etc.
    DrawTopText(deltaTime);

    //Health bar
    DrawPlayerHealthBar(deltaTime);

    //Open Store text
    if(scrapPanelActive == false
    && store.IsStoreActive == false
    && BattleManager::GetInstance()->GetBattleState() == BattleState::Waiting
    && currentTutorialStage >= TutorialStages::StageFinished)
    {
        if (openStoreTextTimer < 1.0f)
            openStoreTextTimer += deltaTime * 0.5f;
    }
    else if(openStoreTextTimer > 0.0f)
        openStoreTextTimer -= deltaTime * 4.0f;

    openStoreTextTimer = Clamp(openStoreTextTimer,0,1.0f);
    Color textColor = Utility::GetColorWithCustomAlpha(PALETTE_PURPLE3,openStoreTextTimer * 255.0f);
    DrawTextMiddleAligned(STR_TUTORIAL_STORE, 40, textColor, 0.5f, 0.8f);


    //Store panel open / close functionality
    if(store.IsStoreActive == false)
    {
        if(IsKeyReleased(KEY_B) && store.IsInputEnabled())
            OpenStoreScreen();
    }
    else
    {
        if ((IsKeyReleased(KEY_B) || IsKeyReleased(KEY_ESCAPE)) && store.IsInputEnabled())
            CloseStoreScreen();
    }

    //Store panel
    if(store.IsStoreActive) {
        DrawStoreScreen(deltaTime);
    }



    //Scrap Panel
    DrawScrapPanel(deltaTime);

    ProcessTutorial(deltaTime);

    DrawPauseScreen(deltaTime);

    //Lose screen
    if(isLoseScreenActive)
        DrawLoseScreen(deltaTime);

    //Fade in on game start
    if(fadeInTimer < 1.0f) {
        fadeInTimer += deltaTime * 0.5f;
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Utility::GetColorWithCustomAlpha(BLACK,255 * (1.0f - fadeInTimer)));
    }
}


void HUD::DrawTopText(float deltaTime) {
    if(store.IsStoreActive == false && BattleManager::GetInstance()->GetWaitTimerState()) {
        if (BattleManager::GetInstance()->GetBattleState() == BattleState::Waiting)
        {
            stringstream waveStr;
            waveStr << STR_WAVE_NAME << BattleManager::GetInstance()->GetCurrentWave() << STR_WAVE_STARTING;
            DrawTextMiddleAligned(waveStr.str(), 40, PALETTE_GRAY1, 0.5f, 0.1f);

            stringstream stream;
            stream << std::fixed << std::setprecision(1) << BattleManager::GetInstance()->GetWaitTimer() << "s";
            DrawTextMiddleAligned(stream.str().c_str(), 40, PALETTE_GRAY1, 0.5f, 0.2f);
        }
        else if (BattleManager::GetInstance()->GetBattleState() == BattleState::BattleStarted)
        {
            stringstream stream;
            stream  << STR_WAVE_NAME << BattleManager::GetInstance()->GetCurrentWave();
            DrawTextMiddleAligned(stream.str().c_str(), 40, PALETTE_RED2, 0.5f, 0.1f);


            stringstream stream2;
            stream2 << STR_ENEMY_COUNT << World::GetInstance()->EnemyFighterCount;
            DrawTextMiddleAligned(stream2.str().c_str(), 40, PALETTE_RED2, 0.5f, 0.2f);
        }
    }
}


void HUD::Render3D(float deltaTime)
{
    DrawGuns(deltaTime);

    if(DEBUG_SHOW_GIZMO) {
        //forward = Vector3Scale(forward,10.0f);
        DrawLine3D(Vector3Zero(), (Vector3) {0, 0, 10}, BLUE);
        DrawLine3D(Vector3Zero(), (Vector3) {0, 10, 0}, GREEN);
        DrawLine3D(Vector3Zero(), (Vector3) {10, 0, 0}, RED);
    }
}


void HUD::Unload()
{
    UnloadModel(playerGunModel);
    UnloadModel(playerGunMuzzleModel);
    UnloadModel(playerShipModel);
    //UnloadTexture(cockpitTexture);
}

void HUD::DrawShopBackgroundShip(float deltaTime)
{
    Vector3 renderPos = {0,0,10};
    Vector3 rotation = {0,store.GetStoreTimer(),0};
    float buyAnimTime = Clamp( store.GetBuyAnimTimer(),0,1);
    float buyAnimScaleMult = 0.75f + (Utility::EaseOutElastic(buyAnimTime) * 0.25f);
    Vector3 scale = Vector3Scale(Vector3One(),0.5f * store.GetStoreAnimScale() * buyAnimScaleMult);
    Renderer::GetInstance()->RenderModelWire(playerShipModel,renderPos, rotation, scale,PALETTE_PURPLE1);
}

void HUD::DrawGuns(float deltaTime) {

    //Draw ship at the background when store is opened
    if(store.IsStoreActive) {

        DrawShopBackgroundShip(deltaTime);
        return;
    }


    Vector3 sway = player->GetSwayInput();
    smoothSway = Vector3Lerp(smoothSway,sway,swaySpeed * deltaTime);

    Vector3 angleSway = Vector3Scale((Vector3){-smoothSway.x,smoothSway.y,smoothSway.y},swayAngleAmount);
    Vector3 positionSway = Vector3Scale((Vector3){-smoothSway.y,-smoothSway.x,-smoothSway.z},swayPositionAmount);

    //Gun Renderings

    //Left Gun
    Vector3 weaponLeftPos = (Vector3){5.5f,-1.0f,5.0f};
    weaponLeftPos = Vector3Add(weaponLeftPos,positionSway);
    Vector3 gunRotationLeft = Vector3Scale((Vector3){0,180,0},DEG2RAD);
    angleSway.z = -angleSway.z;
    gunRotationLeft = Vector3Add(gunRotationLeft,angleSway);
    Renderer::GetInstance()->RenderModelWithWires(playerGunModel,weaponLeftPos, gunRotationLeft,Vector3One(),PALETTE_BLUE2, true);
    Renderer::GetInstance()->RenderModelWithWires(playerGunMuzzleModel,Vector3Add(weaponLeftPos,leftGunShootOffset), gunRotationLeft,Vector3One(),PALETTE_BLUE2, true);




    //Right Gun
    Vector3 weaponRightPos = (Vector3){-5.5f,-1.0f,5.0f};
    weaponRightPos = Vector3Add(weaponRightPos,positionSway);
    Vector3 gunRotationRight = Vector3Scale((Vector3){0,180,0},DEG2RAD);
    gunRotationRight = Vector3Add(gunRotationRight,(Vector3){angleSway.x,-angleSway.y,angleSway.z});
    Vector3 mirroredScale = (Vector3){-1,1,1};
    Renderer::GetInstance()->RenderModelWithWires(playerGunModel,weaponRightPos, gunRotationRight,mirroredScale,PALETTE_BLUE2, true);
    Renderer::GetInstance()->RenderModelWithWires(playerGunMuzzleModel,Vector3Add(weaponRightPos,rightGunShootOffset), gunRotationRight,mirroredScale,PALETTE_BLUE2, true);

    //Nozzle Offset
    leftGunShootOffset = Vector3Lerp(leftGunShootOffset,Vector3Zero(),deltaTime * NozzleReturnToNormalSpeed);
    rightGunShootOffset = Vector3Lerp(rightGunShootOffset,Vector3Zero(),deltaTime * NozzleReturnToNormalSpeed);
}

void HUD::GunShoot(int cannonID) {

    if(cannonID == 0)
        rightGunShootOffset = (Vector3){0,0,shootCannonOffsetAmount};
    else if(cannonID == 1)
        leftGunShootOffset = (Vector3){0,0,shootCannonOffsetAmount};



}

tuple<int, int> HUD::GetMiddleAlignedTextPosition(string text, int fontSize) {
    int textWidth = MeasureText(text.c_str(), fontSize);
    int textHeight = fontSize;
    return make_tuple(textWidth / 2,textHeight / 2);
}

void HUD::DrawTextMiddleAlignedInArea(string text,int fontSize,Color textColor,int areaWidth,int areaHeight,float xPosRatio,float yPosRatio) {

    auto [offsetX, offsetY] = GetMiddleAlignedTextPosition(text,fontSize * screenSizeRatio);

    auto[xPos,yPos] = GetAlignPosition(areaWidth,areaHeight,xPosRatio,yPosRatio);
    xPos -= offsetX;
    yPos -= offsetY;

    DrawTextPure(text,xPos,yPos,fontSize,textColor);
}


void HUD::DrawTextMiddleAligned(string text, int fontSize, Color textColor, float xPosRatio, float yPosRatio) {
    DrawTextMiddleAlignedInArea(text,fontSize,textColor,GetScreenWidth(),GetScreenHeight(),xPosRatio,yPosRatio);
}


void HUD::DrawTextPure(string text, int screenPosX, int screenPosY, int fontSize, Color textColor) {

    //Back Shadow Text
    Color bckColor = Utility::LerpColor(textColor,PALETTE_GRAY5,0.9f);
    bckColor.a = textColor.a;
    DrawText(text.c_str(), screenPosX, screenPosY + bckTextYOffset, fontSize  * screenSizeRatio, bckColor);

    //Real Text
    DrawText(text.c_str(), screenPosX, screenPosY, fontSize * screenSizeRatio, textColor);
}


//Returns the real position when given the position ratios. This the heart of responsive UI
tuple<int,int> HUD::GetAlignPosition(int areaWidth, int areaHeight, float xPosRatio, float yPosRatio) {

    int xPos = (int)((float)areaWidth * xPosRatio);
    int yPos = (int)((float)areaHeight * yPosRatio);

    return make_tuple(xPos,yPos);
}

tuple<int, int> HUD::GetAlignPositionScreen(float xPosRatio, float yPosRatio) {
    return GetAlignPosition(GetScreenWidth(),GetScreenHeight(),xPosRatio,yPosRatio);
}

tuple<int, int, int, int>
HUD::GetAlignPositionRectangle(int areaWidth, int areaHeight, float xStart, float yStart, float xEnd, float yEnd) {

    int xPos = (int) ((float) areaWidth * xStart);
    int yPos = (int) ((float) areaHeight * yStart);
    int width = (int) ((float) areaWidth * xEnd) - xPos;
    int height = (int) ((float) areaHeight * yEnd) - yPos;

    return make_tuple(xPos, yPos, width, height);
}


tuple<int, int, int, int> HUD::GetAlignPositionRectangleScreen(float xStart, float yStart, float xEnd, float yEnd) {
    return GetAlignPositionRectangle(GetScreenWidth(),GetScreenHeight(),xStart,yStart,xEnd,yEnd);
}


void HUD::PlayScrapGainAnim(int scrapAmount) {

    scrapDisappearTimer = 0;

    if (animEaseTimer2 > 0.0f)
        animEaseTimer = 0;
    else
        animEaseTimer = scrapPopupDuration;

    animEaseTimer2 = 0;
}

void HUD::DrawScrapPanel(float deltaTime) {

    scrapPanelActive = false;

    //Why did I write something this complicated ._.
    if(scrapDisappearTimer < scrapDisappearDuration) {

        //Popup anim & panel active
        scrapDisappearTimer += deltaTime;
        scrapActiveScale = 1.0f;
        animEaseTimer2 = 0;
        scrapPanelActive = true;

        if (animEaseTimer < 1.0f + scrapPopupDuration) {
            animEaseTimer += deltaTime;

            if(animEaseTimer >= scrapPopupDuration || animEaseTimer2 > 0.0f)
                animScrapScale = scrapActiveScale * 2.0f + ((1.0f - Utility::EaseOutElastic(animEaseTimer - scrapPopupDuration)));
            else
                animScrapScale = Lerp(0,scrapActiveScale * 2.0f + 1.0f,animEaseTimer / scrapPopupDuration);
        }
    }
    else {

        //Disappear anim
        if (animEaseTimer2 < 1.0f) {
            animEaseTimer2 += deltaTime;
            scrapActiveScale = (1.0f - Utility::EaseOutElastic(animEaseTimer2));
            scrapPanelActive = true;

            if (animEaseTimer < 1.0f)
                animEaseTimer += deltaTime;

            animScrapScale = scrapActiveScale * 2.0f + ((1.0f - Utility::EaseOutElastic(animEaseTimer)));
        }
    }

    if(scrapPanelActive == true) {

        if(abs(animScrapScale) < 0.01f)
            return;

        auto [circleX, circleY] = GetAlignPositionScreen(circlePosX, circlePosY);
        DrawCircle(circleX, circleY, 30 * animScrapScale, PALETTE_GRAY5);
        DrawCircleLines(circleX, circleY, 30 * animScrapScale, PALETTE_PURPLE3);
        Renderer::GetInstance()->RenderScrap((Vector3) {(float) circleX, (float) circleY, 0}, 20.0f * animScrapScale,
                                             true);

        if(animScrapScale > 0.3f) {
            int playerScrapCount = player->GetScrap();
            DrawTextMiddleAligned(to_string(playerScrapCount), 20 * animScrapScale, PALETTE_PURPLE1, circlePosX,
                                  circlePosY);
        }
    }
}

void HUD::DrawPlayerHealthBar(float deltaTime) {

    if(store.IsStoreActive && showHealthBarWhileShopTimer <= 0.0f)
        return;

    if(showHealthBarWhileShopTimer > 0.0f)
        showHealthBarWhileShopTimer-=deltaTime;


    //Hurt FX text
    if(IsKeyPressed(KEY_P)) {
        Player::GetInstance()->Hurt(50);
    }

    auto [x, y,w,h] = GetAlignPositionRectangleScreen(0.2f,0.92f,0.8f,0.98f);
    int health = player->GetHealth();

    //Health bar change smoothing
    if(abs(health-smoothHealth) > 1)
        smoothHealth = Lerp(smoothHealth,health,deltaTime * healthChangeSpeed);
    else
        smoothHealth = health;

    smoothHealth = Clamp(smoothHealth,0,player->GetMaxHealth());

    float healthRatio = smoothHealth / player->GetMaxHealth();

    //Hurt FX
    Color barColor = PALETTE_BLUE5;
    Color tintColor = Utility::GetZeroAlphaColor(PALETTE_RED1);
    Color textColor = PALETTE_BLUE2;
    float animHurtEaseTimer = 1.0f;
    if((GetTime() - player->LastHurtTime) < hurtAnimTime)
    {
        animHurtEaseTimer = ((GetTime() - player->LastHurtTime)  / hurtAnimTime * hurtAnimTime);
        float t = (GetTime() - player->LastHurtTime) / hurtAnimTime;
        barColor = Utility::LerpColor(PALETTE_RED1,barColor,t);
        DrawRectangleLines(x,y,w,h, PALETTE_RED1);

        tintColor = Utility::LerpColor(Utility::GetColorWithCustomAlpha(PALETTE_RED1,hurtTintAlpha),tintColor,t);
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), tintColor);

        textColor = Utility::LerpColor(PALETTE_WHITE,textColor,t);
    }

    float elasticScale = 0.75f + ((Utility::EaseOutElastic(animHurtEaseTimer)) * 0.25f) ;
    float healthSize = ((float) w * healthRatio);

    //Fill
    DrawRectangle(x,y,((float) w),h, PALETTE_BROWN7);
    DrawRectangle(x,y,healthSize,h, barColor);



    //Dots
    dotsTimer += deltaTime * dotsMoveSpeed;
    float xLimit = ((x + w) * healthRatio) / GetScreenWidth();
    float spacing = 0.01f;
    for (float dotX = 0.2f; dotX < xLimit; dotX+= spacing) {
        float s = fabs(sin(dotsTimer + dotX * dotsSizeDiff));

        Color dotColor = Utility::GetColorWithCustomAlpha(PALETTE_BLUE4, (s * 255));

        for (float dotY = 0.92f; dotY < 0.97f; dotY += spacing) {

            float animatedYPos = dotY - ((1.0f - s) * 0.015f);
            auto [realDotX, animatedDotY] = GetAlignPositionScreen(dotX, animatedYPos);
            DrawRectangle(realDotX, animatedDotY, 2, 2, dotColor);
        }
    }

    //Lines
    DrawRectangleLines(x,y,healthSize,h, PALETTE_BLUE2);
    DrawRectangleLines(x,y ,w,h, PALETTE_BLUE2);

    //Text
    DrawTextMiddleAligned(to_string((int)smoothHealth),(((float)40) * elasticScale),textColor,0.5f,0.95f);


}


void HUD::OpenStoreScreen() {

    if((BattleManager::GetInstance()->GetBattleState() != BattleState::Waiting))
        return;

    PlayScrapGainAnim(0);
    player->SetCanMove(false);
    player->SetCanRotate(false);
    store.OpenStore();
    BattleManager::GetInstance()->SetWaitTimerState(false);
}

void HUD::CloseStoreScreen() {

    store.CloseStore();
}

void HUD::DrawStoreScreen(float deltaTime) {

    scrapDisappearTimer = 0;

    auto [panelPosX, panelPosY, panelWidth,panelHeight] = GetAlignPositionRectangleScreen(0.3f,0.3f,0.7f,0.7f);


    CanvasParams params = {panelPosX,panelPosY,panelWidth,panelHeight,screenSizeRatio,bckTextYOffset-1};
    store.RenderAndUpdate(deltaTime,params);
}

void HUD::StoreCloseAnimFinished() {

    scrapDisappearTimer = scrapDisappearDuration;
    player->SetCanMove(true);
    player->SetCanRotate(true);
    BattleManager::GetInstance()->SetWaitTimerState(true);
}

void HUD::PlayerBuyAnim(int itemIndex) {

    if(itemIndex == 0 || itemIndex == 4)
    {
        showHealthBarWhileShopTimer = 1.0f;
    }

    PlayScrapGainAnim(0);
}


void HUD::PlayerDied() {

    isLoseScreenActive = true;
    BattleManager::GetInstance()->SetWaitTimerState(false);
}

void HUD::DrawLoseScreen(float deltaTime) {


    loseScreenTimer += deltaTime;

    float t = Utility::Clamp01(loseScreenTimer / loseScreenAnimDuration);
    t = pow(t,4);

    if(t < 1)
    {
        //Lines
        spawnNewLoseLineTimer += deltaTime;
        if(spawnNewLoseLineTimer > 0.01f * loseScreenAnimDuration) {
            loseScreenLines.push_back(make_tuple(GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenWidth())));
        }

        for (int i = 0; i < loseScreenLines.size(); ++i) {

            auto[x, x2] = loseScreenLines[i];

            DrawLine(x,0,x2,GetScreenHeight(),loseScreenFadeColor);
        }
    }


    DrawRectangle(0,0,GetScreenWidth(),(float)GetScreenHeight() * 0.5f * t,loseScreenFadeColor);
    DrawRectangle(0,GetScreenHeight() - (float)GetScreenHeight() * 0.5f * t,GetScreenWidth(),(float)GetScreenHeight() * 0.5f * t,loseScreenFadeColor);



    if(t >= 1)
    {
        //Lose texts
        float xSin = sin(loseScreenTimer) * 0.05f;

        float tintT = Utility::Clamp01(loseScreenTimer - loseScreenAnimDuration);
        Color tint = Utility::LerpColor(Utility::GetZeroAlphaColor(PALETTE_PURPLE2),PALETTE_PURPLE2,tintT);
        Color tint2 = Utility::LerpColor(Utility::GetZeroAlphaColor(PALETTE_RED1),PALETTE_RED1,tintT);

        DrawTextMiddleAligned(STR_LOSE_SCREEN_TITLE, 60, tint2, 0.5f + xSin, 0.2f);

        stringstream waveStr;
        waveStr << STR_LOSE_SCREEN_SURVIVED_FOR << " " << to_string(BattleManager::GetInstance()->GetCurrentWave()) << " " << STR_WAVE_NAME_PLURAL;
        DrawTextMiddleAligned(waveStr.str(), 40, tint, 0.5f, 0.35f);

        DrawTextMiddleAligned(STR_GAME_NAME, 40, tint, 0.5f, 0.6f);
        DrawTextMiddleAligned(STR_CREDITS, 40, tint, 0.5f, 0.65f);

        //Exit text
#if defined(PLATFORM_DESKTOP)
        DrawTextMiddleAligned(STR_LOSE_SCREEN_QUIT_KEY, 40, tint, 0.5f, 0.9f);
#endif

#if defined(PLATFORM_WEB)
        DrawTextMiddleAligned(STR_LOSE_SCREEN_QUIT_KEY_WEB, 40, tint, 0.5f, 0.9f);
#endif

    }
    else
    {
        //Particles
        createLoseParticleTimer+ deltaTime;
        if(createLoseParticleTimer > 0.2f)
        {
            createLoseParticleTimer = 0;
            ParticleManager::GetInstance()->CreateShipSpawnFX(player->Position,TEAM_PLAYER);
            ParticleManager::GetInstance()->CreateScrapGlitter(player->Position,PALETTE_RED1);
        }
    }


}

void HUD::ProcessTutorial(float deltaTime) {

    if(currentTutorialStage == TutorialStages::StageFinished)
        return;

    Player* player = Player::GetInstance();

    newTutorialTimer+=deltaTime;
    float t = Utility::Clamp01(newTutorialTimer / 1.0f);
    Color tutorialTextTint = Utility::LerpColor(PALETTE_PURPLE1,PALETTE_PURPLE2,t);

    //Rotate
    if(currentTutorialStage == TutorialStages::StageBasics)
    {
        DrawTextMiddleAligned(STR_TUTORIAL_BASICS1, 40, tutorialTextTint, 0.5f, 0.1f);
        DrawTextMiddleAligned(STR_TUTORIAL_BASICS2, 40, tutorialTextTint, 0.5f, 0.2f);

        if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            tutorialLookTimer += deltaTime;
            if (tutorialLookTimer > 1.0f) {
                currentTutorialStage = TutorialStages::StageThrust;
                newTutorialTimer = 0;
            }
        }
    }

    //Thrust
    if(currentTutorialStage == TutorialStages::StageThrust)
    {
        DrawTextMiddleAligned(STR_TUTORIAL_THRUST, 40, tutorialTextTint, 0.5f, 0.1f);
        DrawTextMiddleAligned(STR_TUTORIAL_THRUST2, 40, tutorialTextTint, 0.5f, 0.2f);

        if(player->GetVelocityRatioToMaxValue() > 0.9f)
        {
            newTutorialTimer = 0;
            currentTutorialStage = TutorialStages::StageShoot;
        }
    }

    //Shoot
    if(currentTutorialStage == TutorialStages::StageShoot)
    {
        DrawTextMiddleAligned(STR_TUTORIAL_SHOOT, 40, tutorialTextTint, 0.5f, 0.1f);

        if(World::GetInstance()->ActiveBulletCount > 10) {
            tutorialShootTimer += deltaTime;
            if (tutorialShootTimer > 1.0f) {
                newTutorialTimer = 0;
                currentTutorialStage = TutorialStages::StageScrap;
            }
        }
    }

    //Scrap
    if(currentTutorialStage == TutorialStages::StageScrap)
    {
        DrawTextMiddleAligned(STR_TUTORIAL_SCRAP, 40, tutorialTextTint, 0.5f, 0.1f);

        if(player->GetScrap() > 150)
        {
            newTutorialTimer = 0;
            currentTutorialStage = TutorialStages::StageStore;
        }
    }


    //Store
    if(currentTutorialStage == TutorialStages::StageStore)
    {
        if(store.IsStoreActive)
            tutorialStoreOpened = true;
        else
        {
            DrawTextMiddleAligned(STR_TUTORIAL_STORE_HOWTO1, 40, tutorialTextTint, 0.5f, 0.1f);
            DrawTextMiddleAligned(STR_TUTORIAL_STORE_HOWTO2, 40, tutorialTextTint, 0.5f, 0.2f);
        }

        if(tutorialStoreOpened && store.IsStoreActive == false)
        {
            newTutorialTimer = 0;
            currentTutorialStage = TutorialStages::StageWaves;
            BattleManager::GetInstance()->SetWaitTimerState(true);
        }
    }


    //Waves
    if(currentTutorialStage == TutorialStages::StageWaves)
    {
        DrawTextMiddleAligned(STR_TUTORIAL_WAVES1, 40, tutorialTextTint, 0.5f, 0.3f);
        DrawTextMiddleAligned(STR_TUTORIAL_WAVES2, 40, tutorialTextTint, 0.5f, 0.35f);

        if(BattleManager::GetInstance()->GetWaitTimer() < 45.0f)
        {
            newTutorialTimer = 0;
            currentTutorialStage = TutorialStages::StageFinished;
        }
    }


    //If player tries to cheat
    if(player->GetScrap() > 600 || (currentTutorialStage < TutorialStages::StageStore && store.IsStoreActive))
    {
        BattleManager::GetInstance()->SetWaitTimerState(true);
        currentTutorialStage = TutorialStages::StageFinished;
    }
}

void HUD::DrawPauseScreen(float deltaTime) {

    if(isGamePaused == true)
    {
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Utility::GetColorWithCustomAlpha(BLACK,200));
        DrawTextMiddleAligned(STR_PAUSED_TITLE, 40, PALETTE_GRAY2, 0.5f, 0.3f);
        DrawTextMiddleAligned(STR_PAUSED_RESUME, 40, PALETTE_GRAY2, 0.5f, 0.7f);
        DrawTextMiddleAligned(STR_PAUSED_QUIT, 40, PALETTE_GRAY2, 0.5f, 0.8f);
    }
}

void HUD::ChangePauseState(bool newState) {

    isGamePaused = newState;
}
