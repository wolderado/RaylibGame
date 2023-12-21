//
// Created by Salih on 10-Dec-23.
//

#ifndef SRC_GLOBALS_H
#define SRC_GLOBALS_H


#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"


static const int ScreenWidth = 1280;
static const int ScreenHeight = 720;
static const int RenderWidth = 640;
static const int RenderHeight = 360;
static float TimeSpeed = 1;


class Utility
{
public:
    static Color LerpColor(Color color1,Color color2,float amount){
        Color result;
        result.r = Lerp(color1.r,color2.r,amount);
        result.g = Lerp(color1.g,color2.g,amount);
        result.b = Lerp(color1.b,color2.b,amount);
        result.a = Lerp(color1.a,color2.a,amount);
        return result;
    }
};

#endif //SRC_GLOBALS_H