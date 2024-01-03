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


static const int DefaultScreenWidth = 1280;
static const int DefaultScreenHeight = 720;
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

};

#endif //SRC_GLOBALS_H