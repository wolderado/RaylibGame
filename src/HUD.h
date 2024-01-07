//
// Created by Salih on 16-Dec-23.
//

#ifndef SRC_HUD_H
#define SRC_HUD_H
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>

#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"
#include "Color.h"
#include "Globals.h"
#include "Renderer.h"
#include "Player.h"
#include "World.h"
#include "BattleManager.h"
#include "Store.h"


const bool DEBUG_SHOW_GIZMO = false;



class HUD {
public:
    //Positions
    const float circlePosX = 0.5f;
    const float circlePosY = 0.8f;

    void Init(shared_ptr<Player> targetPlayer);
    void Render(float deltaTime);
    void Render3D(float deltaTime);
    void Unload();
    void GunShoot(int cannonID);
    void PlayScrapGainAnim(int scrapAmount);

private:
    const float swayAngleAmount = 5.0f;
    const float swayPositionAmount = 30.0f;
    const float swaySpeed = 5.0f;
    const float NozzleReturnToNormalSpeed = 10.0f;
    const float shootCannonOffsetAmount = -2.0f;
    const float scrapDisappearDuration = 2.0f;
    const float scrapPopupDuration = 0.25f;
    const unsigned char hurtTintAlpha = 50;
    const float hurtAnimTime = 1.0f;
    const float dotsMoveSpeed = -1.5f;
    const float dotsSizeDiff = 10.0f;
    const float healthChangeSpeed = 7.0f;
    //Text Shadow Offset
    const int bckTextYOffset = 4;
    const float loseScreenAnimDuration = 3.0f;
    const Color loseScreenFadeColor = PALETTE_PURPLE5;

    shared_ptr<Player> player;
    Store store;
    Texture cockpitTexture;
    Model playerGunModel;
    Model playerGunMuzzleModel;
    Model playerShipModel;
    Vector3 smoothSway;
    Vector3 leftGunShootOffset = {0,0,0};
    Vector3 rightGunShootOffset = {0,0,0};
    float animScrapScale = 1;
    float animEaseTimer = 0.0f;
    float animEaseTimer2 = 0.0f;
    float scrapDisappearTimer = 999;
    float scrapActiveScale = 0.0f;
    bool scrapPanelActive = false;
    float healthBarYScale = 1.0f;
    float dotsTimer = 0;
    float smoothHealth = 0;
    float screenSizeRatio = 1.0f;
    float showHealthBarWhileShopTimer = 0.0f;
    float openStoreTextTimer = 0;
    bool isLoseScreenActive = false;
    float loseScreenTimer = 0;
    float createLoseParticleTimer = 0;
    float spawnNewLoseLineTimer = 0;

    void DrawGuns(float deltaTime);
    void DrawScrapPanel(float deltaTime);
    void DrawPlayerHealthBar(float deltaTime);
    tuple<int,int> GetMiddleAlignedTextPosition(string text, int fontSize);
    tuple<int,int> GetAlignPosition(int areaWidth,int areaHeight,float xPosRatio,float yPosRatio);
    tuple<int,int> GetAlignPositionScreen(float xPosRatio,float yPosRatio);
    tuple<int,int,int,int> GetAlignPositionRectangle(int areaWidth,int areaHeight,float xStart,float yStart,float xEnd,float yEnd);
    tuple<int,int,int,int> GetAlignPositionRectangleScreen(float xStart,float yStart,float xEnd,float yEnd);
    void DrawTextMiddleAligned(string text,int fontSize,Color textColor,float xPosRatio,float yPosRatio);
    void DrawTextMiddleAlignedInArea(string text,int fontSize,Color textColor,int areaWidth,int areaHeight,float xPosRatio,float yPosRatio);
    void DrawTextPure(string text,int screenPosX,int screenPosY,int fontSize,Color textColor);
    void OpenStoreScreen();
    void DrawStoreScreen(float deltaTime);
    void CloseStoreScreen();
    void StoreCloseAnimFinished();
    void PlayerBuyAnim(int itemIndex);
    void DrawShopBackgroundShip(float deltaTime);
    void DrawTopText(float deltaTime);
    void DrawLoseScreen(float deltaTime);
    void PlayerDied();
    vector<tuple<int,int>> loseScreenLines;

    //STRING KEYS for easy localization for the future
    string STR_WAVE_NAME = "Wave ";
    string STR_WAVE_NAME_PLURAL = "Waves";
    string STR_WAVE_STARTING = " is coming in: ";
    string STR_ENEMY_COUNT = "Enemy left: ";
    string STR_TUTORIAL_STORE = "Press B to Open Store";
    string STR_LOSE_SCREEN_TITLE = "Game Over";
    string STR_LOSE_SCREEN_SURVIVED_FOR = "Survived For";
    string STR_LOSE_SCREEN_QUIT_KEY = "Press ESC to Quit";
    string STR_LOSE_SCREEN_QUIT_KEY_WEB = "Press F5 to Restart";
    string STR_GAME_NAME = "AstroBlast 3D";
    string STR_CREDITS = "Made by Wolderado";
};


#endif //SRC_HUD_H
