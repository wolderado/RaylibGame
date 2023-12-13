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
#include "GameObjectManager.h"



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
static void UpdateDrawFrame(void);      // Update and Draw one frame
Player player;
Renderer* renderer;
GameObjectManager gameObjectManager;


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{


    SetTraceLogLevel(LOG_NONE);

    InitWindow(ScreenWidth, ScreenHeight, "Space Battle Game");

    //Divider
    std::cout << "---------------------------------------" << "\n";

    player.Init();
    renderer = Renderer::GetInstance();
    renderer->InitRenderer(&player);


    for (int i = 0; i < 300; ++i) {
        Vector3 rndPos = { (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
        gameObjectManager.CreateNewAsteroid(rndPos);
    }



    
    // Render texture to draw full screen, enables screen scaling
    target = LoadRenderTexture(RenderWidth, RenderHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

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
    // Render game screen to a texture,
    BeginTextureMode(target);
        ClearBackground(PALETTE_WHITE);
        renderer->RenderBackground();

        //3D Rendering starts here
        BeginMode3D(*player.GetCamera());
        renderer->RenderAtmosphere();

        Color dotColor = PALETTE_BLUE2;
        dotColor = ColorAlpha(dotColor,0.05f);
        DrawCube((Vector3){ 5.0f, 0.0f, 5.0f }, 5.0f, 1.0f, 1.0f, FAKE_TRANSPARENT1);
        DrawCubeWires((Vector3){ 5.0f, 0.0f, 5.0f }, 5.0f, 1.0f, 1.0f, PALETTE_BLUE2);

        DrawCubeWires((Vector3){ 5.0f, 0.0f, -15.0f }, 1.0f, 2.0f, 1.0f, GREEN);
        DrawSphereWires((Vector3){ 0.0f, 20.0f, 0.0f }, 1.0f, 5, 5, PALETTE_RED2);

        player.Update(deltaTime);
        gameObjectManager.UpdateAll(deltaTime);
        EndMode3D();
    EndTextureMode();




    
    // Render to screen (main framebuffer)
    BeginDrawing();
    ClearBackground(PALETTE_WHITE);
    // Draw render texture to screen, scaled if required
    DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, (Rectangle){ 0, 0, (float)ScreenWidth, (float)ScreenHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);

    DrawFPS(10, 10);
    // TODO: Draw UI


    //DrawText("IT WORKS!", GetScreenWidth()/2, GetScreenHeight()/2, 100, RED);
    EndDrawing();
    //----------------------------------------------------------------------------------  
}