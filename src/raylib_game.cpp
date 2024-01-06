/*******************************************************************************************
*
*   raylib gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 5.0
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../raylib/src/raylib.h"
#include "Player.h"
#include "Renderer.h"
#include "Color.h"
#include "World.h"
#include "HUD.h"
#include "ParticleManager.h"
#include "BattleManager.h"


#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif


static RenderTexture2D target = { 0 };  // Render texture to render our game
static RenderTexture2D targetHUD = { 0 };  // Render texture to render our game
static void UpdateDrawFrame(void);      // Update and Draw one frame
static float TimeSpeed = 1;
shared_ptr<Player> player;
Renderer* renderer;
World* world;
BulletManager* bulletManager;
HUD hud;
ParticleManager* particleManager;
BattleManager* battleManager;



//TODO Optimizations:
//Grid based collision detection for both game objects and bullets
//Cache all required models and don't load models within game object
//Implement camera to angles conversion for Player.cpp rotation
//Fix muzzle particle misaligning with camera (fix local space particles)



int main(void)
{
    SetTraceLogLevel(LOG_NONE);

#if defined(PLATFORM_DESKTOP)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
#endif

    InitWindow(DefaultScreenWidth, DefaultScreenHeight, "Space Battle Game");

    //Divider
    std::cout << "---------------------------------------" << "\n";

    //Collect Instances
    world = World::GetInstance();
    bulletManager = BulletManager::GetInstance();
    particleManager = ParticleManager::GetInstance();
    renderer = Renderer::GetInstance();
    battleManager = BattleManager::GetInstance();

    //Init world first
    world->GenerateWorld();

    //Setup Player
    player = shared_ptr<Player>(Player::GetInstance());
    player->Init();
    player->Name = "Player";
    world->InitObject(player);

    //Init
    renderer->InitRenderer(player->GetCamera());
    hud.Init(player);
    bulletManager->Init();
    battleManager->Init();




    // Render texture to draw full screen, enables screen scaling
    target = LoadRenderTexture(RenderWidth, RenderHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    targetHUD = LoadRenderTexture(RenderWidth, RenderHeight);
    SetTextureFilter(targetHUD.texture, TEXTURE_FILTER_POINT);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    renderer->Unload();
    UnloadRenderTexture(target);
    
    // TODO: Unload all loaded resources at this point

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    // Update
    float deltaTime = GetFrameTime() * TimeSpeed;

    // Draw
    //----------------------------------------------------------------------------------
    BeginTextureMode(target);
        ClearBackground(SKY_COLOR);
        renderer->RenderBackground();

        BeginMode3D(*player->GetCamera());

            renderer->RenderAtmosphere(player->GetVelocityRatioToMaxValue(),player->GetVelocity());
            world->GetInstance()->UpdateAll(deltaTime);
            battleManager->UpdateAI(deltaTime);
            renderer->Update(deltaTime);

            //Billboard Renders
            renderer->BeginAlphaCutoff();
                bulletManager->UpdateAndRender(deltaTime);
                particleManager->UpdateAndRender(deltaTime);
            renderer->EndAlphaCutoff();
        EndMode3D();
    EndTextureMode();


    //HUD Camera Render
    BeginTextureMode(targetHUD);
        ClearBackground((Color){0,0,0,0});
        BeginMode3D(*player->GetHUDCamera());
            hud.Render3D(deltaTime);
        EndMode3D();
    EndTextureMode();



    
    // Render to screen (main framebuffer)
    BeginDrawing();
    ClearBackground(SKY_COLOR);

    // Draw render texture to screen
    DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    DrawTexturePro(targetHUD.texture, (Rectangle){ 0, 0, (float)targetHUD.texture.width, -(float)targetHUD.texture.height }, (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, (Vector2){ 0, 0 }, 0.0f, WHITE);

    // Draw UI
    DrawFPS(10, 10);
    DrawText("Version 24",10, 40, 20, PALETTE_YELLOW1);
    hud.Render(deltaTime);


    //DrawText("IT WORKS!", GetScreenWidth()/2, GetScreenHeight()/2, 100, RED);
    EndDrawing();
    //----------------------------------------------------------------------------------  
}