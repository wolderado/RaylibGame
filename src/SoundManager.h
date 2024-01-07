//
// Created by Salih on 07-Jan-24.
//

#ifndef SRC_SOUNDMANAGER_H
#define SRC_SOUNDMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../../raylib/src/raylib.h"
#include "../../raylib/src/raymath.h"
#include "../../raylib/src/rcamera.h"





using namespace std;

class SoundManager {
public:
    const static string SoundsPath;
    static void Init(Camera3D* mainCamera);
    static void Update(float deltaTime,bool isGamePaused);
    static void LoadSounds();
    static void PlaySoundLooped(string soundKey);
    static void StopSoundLooped(string soundKey);
    static void PlaySound2D(string soundKey,float pitch = 1.0f,float volume = 1.0f, float pan = 0.5f);
    static void PlaySound2DRandomized(string soundKey,float pitchRange = 0.2f,float volumeRange = 0.2f, float pan = 0.5f);
    static void PlaySound3D(string soundKey, Vector3 position,float maxDistance = 1000.0f);
    static void Unload();


private:
    static map<string,Sound> allSounds;
    static float maxVolume;
    static vector<string> loopingSounds;
    static Camera3D* cam;
};


#endif //SRC_SOUNDMANAGER_H
