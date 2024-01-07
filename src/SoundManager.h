//
// Created by Salih on 07-Jan-24.
//

#ifndef SRC_SOUNDMANAGER_H
#define SRC_SOUNDMANAGER_H

#include <iostream>
#include <string>
#include <map>
#include "../../raylib/src/raylib.h"

#define NOGDI
#include <dirent.h>




using namespace std;

class SoundManager {
public:
    const static string SoundsPath;

    static void LoadSounds();
    static void PlaySound(string soundKey);
    static void Unload();

private:
    static map<string,Sound> allSounds;
};


#endif //SRC_SOUNDMANAGER_H
