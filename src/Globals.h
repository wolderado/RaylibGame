//
// Created by Salih on 10-Dec-23.
//

#ifndef SRC_GLOBALS_H
#define SRC_GLOBALS_H


#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"

#define VECTOR3_UP (Vector3){0,1,0}
#define VECTOR3_DOWN (Vector3){0,-1,0}
#define VECTOR3_LEFT (Vector3){-1,0,0}
#define VECTOR3_RIGHT (Vector3){1,0,0}
#define VECTOR3_FORWARD (Vector3){0,0,1}
#define VECTOR3_BACKWARD (Vector3){0,0,-1}




//Static Game Parameters
#define STAT_HEALTH_FIGHTER 25
#define STAT_HEALTH_PLAYER 200
#define STAT_HEALTH_PLAYER_DAMAGE_REDUCTION 0.25f
#define STAT_SHOOT_COOLDOWN_FIGHTER 0.15
#define STAT_SHOOT_COOLDOWN_PLAYER 0.05
#define STAT_BULLET_DAMAGE_FIGHTER 4.0f
#define STAT_BULLET_DAMAGE_PLAYER 4.0f

#define GAME_SHOP_WAIT_TIME 60.0f
#define MAP_ASTEROID_COUNT 100
#define REWARD_SCRAP_FIGHTER 25


//More parameters
static const int DefaultScreenWidth = 1280;
static const int DefaultScreenHeight = 720;
static const int RenderWidth = 640;
static const int RenderHeight = 360;
static const float MapSizeX = 300;
static const float MapSizeY = 300;
static const float MapSizeZ = 300;
static const float AbsoluteMapLimitX = 500;
static const float AbsoluteMapLimitY = 500;
static const float AbsoluteMapLimitZ = 500;

const float STAT_UPGRADE_PER_LEVEL_DAMAGE = 0.15f;
const float STAT_UPGRADE_PER_LEVEL_HEALTH = 0.25f;
const float STAT_UPGRADE_PER_LEVEL_SPEED = 0.25f;

const int EnemyCountIncreasePerWave = 5;
const float EnemyHealthIncreasePerWave = 1.05f;


const bool DEBUG_SHOW_COLLISION_AREA = false;
const bool DEBUG_SHOW_SHOOT_RANGE = false;
const bool DEBUG_DISABLE_AI = false;
const bool DEBUG_SHOW_FIGHTER_TARGETS = false;
const bool DEBUG_DISABLE_SCRAP_DROP = false;


class Utility
{
public:
    static Color LerpColor(Color color1,Color color2,float amount){
        amount = fmin(1.0f,fmax(0.0f,amount));

        Color result;
        result.r = Lerp(color1.r,color2.r,amount);
        result.g = Lerp(color1.g,color2.g,amount);
        result.b = Lerp(color1.b,color2.b,amount);
        result.a = Lerp(color1.a,color2.a,amount);
        return result;
    }

    static Color GetZeroAlphaColor(Color color)
    {
        return (Color){color.r,color.g,color.b,0};
    }

    static Color GetColorWithCustomAlpha(Color color,unsigned char alpha)
    {
        return (Color){color.r,color.g,color.b,alpha};
    }

    static Vector3 GetRandomDirection() {
        Vector3 result;
        result.x = GetRandomValue(-100,100);
        result.y = GetRandomValue(-100,100);
        result.z = GetRandomValue(-100,100);
        return Vector3Normalize(result);
    }

    static Vector3 Vector3FromLocalToGlobal(Vector3 localPosition, Vector3 angles,Vector3 globalPosition)
    {
        Vector3 pos = localPosition;

        pos = Vector3RotateByAxisAngle(pos,(Vector3){0,1,0},angles.y);
        pos = Vector3RotateByAxisAngle(pos,(Vector3){1,0,0},angles.x);
        pos = Vector3RotateByAxisAngle(pos,(Vector3){0,0,1},angles.z);

        //TODO: Implement local rotation

        pos = Vector3Add(pos,globalPosition);
        return pos;
    }

    //Edited from: https://stackoverflow.com/questions/12435671/quaternion-lookat-function
    //TODO: Implement a function that takes UP vector so it doesn't cause unnecessary rolls
    static Quaternion LookAt(Vector3 sourcePoint, Vector3 destPoint)
    {
        if(Vector3Equals(sourcePoint,destPoint))
            return QuaternionIdentity();

        Vector3 forwardVector = Vector3Normalize(Vector3Subtract(destPoint , sourcePoint));

        float dot = Vector3DotProduct(VECTOR3_FORWARD, forwardVector);

        if (abs(dot - (-1.0f)) < 0.000001f)
        {
            return (Quaternion){0, 1, 0, 3.1415926535897932f};
        }
        if (abs(dot - (1.0f)) < 0.000001f)
        {
            return QuaternionIdentity();
        }

        float rotAngle = acos(dot);
        Vector3 rotAxis = Vector3CrossProduct(VECTOR3_FORWARD, forwardVector);
        rotAxis = Vector3Normalize(rotAxis);
        return QuaternionFromAxisAngle(rotAxis, rotAngle);
    }

    static Vector3 GetRandomPosInsideMap()
    {
        Vector3 result;
        result.x = GetRandomValue(-MapSizeX,MapSizeX);
        result.y = GetRandomValue(-MapSizeY,MapSizeY);
        result.z = GetRandomValue(-MapSizeZ,MapSizeZ);
        return result;
    }

    static bool IsInsideMapArea(Vector3 position)
    {
        return (position.x < MapSizeX && position.x > -MapSizeX && position.y < MapSizeY && position.y > -MapSizeY && position.z < MapSizeZ && position.z > -MapSizeZ);
    }

    static bool IsOutOfAbsoluteMapArea(Vector3 position)
    {
        return (position.x > AbsoluteMapLimitX || position.x < -AbsoluteMapLimitX || position.y > AbsoluteMapLimitY || position.y < -AbsoluteMapLimitY || position.z > AbsoluteMapLimitZ || position.z < -AbsoluteMapLimitZ);
    }

    static float Clamp01(float t)
    {
        return fmin(1.0f,fmax(0.0f,t));
    }

    static float EaseOutElastic(float t)
    {
        t = fmin(1.0f, fmax(0.0f, t));
        float t2 = (t - 1) * (t - 1);
        float result = 1.0f - t2 * t2 * cos( t * PI * 1.5f );
        result = fmin(2.0f, fmax(0.0f, result));
        return result;
    }
};


#endif //SRC_GLOBALS_H