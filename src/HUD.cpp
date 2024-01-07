//
// Created by Salih on 16-Dec-23.
//

#include "HUD.h"

void HUD::Init(shared_ptr<Player> targetPlayer)
{
    player = shared_ptr<Player>(targetPlayer);
    playerGunModel = LoadModel("resources/PlayerGun.glb");
    playerGunMuzzleModel = LoadModel("resources/PlayerGunMuzzle.glb");


    player->OnShoot.AddListener(std::bind(&HUD::GunShoot, this, std::placeholders::_1));
    player->OnScrapGain.AddListener(std::bind(&HUD::PlayScrapGainAnim, this, std::placeholders::_1));
}


void HUD::Render(float deltaTime)
{
/*    DrawTexturePro(cockpitTexture, (Rectangle){0,0,(float)cockpitTexture.width,(float)cockpitTexture.height},
                   (Rectangle){0,0,ScreenWidth,ScreenHeight}, (Vector2){0,0}, 0, WHITE);*/

    int middleX = GetScreenWidth() / 2;
    int middleY = GetScreenHeight() / 2;

    //Wave Timers
    if(BattleManager::GetInstance()->GetBattleState() == BattleState::Waiting) {

        DrawTextMiddleAligned(STR_WAVE_STARTING.c_str(),40,PALETTE_GRAY1,0.5f,0.1f);

        stringstream stream;
        stream << std::fixed << std::setprecision(1) << BattleManager::GetInstance()->GetWaitTimer() << "s";
        DrawTextMiddleAligned(stream.str().c_str(),40,PALETTE_GRAY1,0.5f,0.2f);
    }
    else if(BattleManager::GetInstance()->GetBattleState() == BattleState::BattleStarted) {
        stringstream stream;
        stream << STR_ENEMY_COUNT << World::GetInstance()->EnemyFighterCount;
        DrawTextMiddleAligned(stream.str().c_str(),40,PALETTE_RED2,0.5f,0.1f);
    }


    //Draw Scrap Circle
    DrawScrapPanel(deltaTime);

    DrawPlayerHealthBar(deltaTime);
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
    //UnloadTexture(cockpitTexture);
}

void HUD::DrawGuns(float deltaTime) {

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
    int middlePosX = areaWidth / 2;
    int middlePosY = areaHeight / 2;
    auto [offsetX, offsetY] = GetMiddleAlignedTextPosition(text,fontSize);

    auto[xPos,yPos] = GetAlignPosition(areaWidth,areaHeight,xPosRatio,yPosRatio);
    xPos -= offsetX;
    yPos -= offsetY;

    DrawBackText(text, xPos, yPos, fontSize, Utility::LerpColor(textColor,PALETTE_GRAY5,0.9f));
    DrawText(text.c_str(), xPos, yPos, fontSize, textColor);

}


void HUD::DrawTextMiddleAligned(string text, int fontSize, Color textColor, float xPosRatio, float yPosRatio) {
    DrawTextMiddleAlignedInArea(text,fontSize,textColor,GetScreenWidth(),GetScreenHeight(),xPosRatio,yPosRatio);
}


void HUD::DrawBackText(string text, int realPosX, int realPosY, int fontSize, Color textColor) {

    DrawText(text.c_str(), realPosX, realPosY + bckTextYOffset, fontSize, textColor);
}


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
    animEaseTimer = 0;
    scrapDisappearTimer = 0;
}

void HUD::DrawScrapPanel(float deltaTime) {

    scrapPanelActive = false;
    if(scrapDisappearTimer < scrapDisappearDuration) {
        //Scrap Panel active timer
        scrapDisappearTimer += deltaTime;
        scrapActiveScale = 1.0f;
        animEaseTimer2 = 0;
        scrapPanelActive = true;
    }
    else {
        //Disappear anim
        if (animEaseTimer2 < 1.0f) {
            animEaseTimer2 += deltaTime;
            scrapActiveScale = (1.0f - Utility::EaseOutElastic(animEaseTimer2));
            scrapPanelActive = true;
        }
    }

    if(scrapPanelActive == true) {
        auto [circleX, circleY] = GetAlignPositionScreen(circlePosX, circlePosY);
        DrawCircle(circleX, circleY, 30 * animScrapScale, PALETTE_GRAY5);
        DrawCircleLines(circleX, circleY, 30 * animScrapScale, PALETTE_PURPLE3);
        Renderer::GetInstance()->RenderScrap((Vector3) {(float) circleX, (float) circleY, 0}, 20.0f * animScrapScale,
                                             true);

        if(animScrapScale > 0.5f) {
            int playerScrapCount = player->GetScrap();
            DrawTextMiddleAligned(to_string(playerScrapCount), 20 * animScrapScale, PALETTE_PURPLE1, circlePosX,
                                  circlePosY);
        }

        if (animEaseTimer < 1.0f)
            animEaseTimer += deltaTime;

        animScrapScale = scrapActiveScale * 2.0f + ((1.0f - Utility::EaseOutElastic(animEaseTimer)));
    }
}

void HUD::DrawPlayerHealthBar(float deltaTime) {


    if(IsKeyPressed(KEY_P))
    {
        player->Hurt(10);
    }

    auto [x, y,w,h] = GetAlignPositionRectangleScreen(0.2f,0.92f,0.8f,0.98f);
    int health = player->GetHealth();
    smoothHealth = Lerp(smoothHealth,health,deltaTime * healthChangeSpeed);
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
