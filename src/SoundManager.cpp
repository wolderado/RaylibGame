//
// Created by Salih on 07-Jan-24.
//

#include "SoundManager.h"

#define NOGDI
#include <dirent.h>

// Initialize the static member variable
std::map<string,Sound> SoundManager::allSounds;
float SoundManager::maxVolume = 1.0f;
vector<string> SoundManager::loopingSounds;
Camera* SoundManager::cam = nullptr;


void SoundManager::LoadSounds() {

    cout << "Loading sounds..." << endl;

    // Open the directory
    string folderPath  = "resources/Sounds";



    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(folderPath.c_str())) != NULL) {
        // Iterate over each file in the directory
        while ((entry = readdir(dir)) != NULL) {
            std::string fileName = entry->d_name;

            // Check if the file has a .wav extension
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".wav") {
                // Load the sound
                std::string filePath = folderPath + "/" + fileName;
                Sound sound = LoadSound(filePath.c_str());

                // Insert the sound into the map with the file name (without extension) as the key
                string key = fileName.substr(0, fileName.size() - 4);
                allSounds[key] = sound;

                //std::cout << "Loaded sound: " << fileName << std::endl;
            }
        }

        closedir(dir);
    } else {
        std::cerr << "Error opening directory " << folderPath << std::endl;
    }
}

void SoundManager::Unload() {
    // Unload sounds when the program exits
    for (auto& pair : allSounds) {
        UnloadSound(pair.second);
    }
}

void SoundManager::PlaySound2D(string soundKey,float pitch , float volume, float pan) {

    if(allSounds.find(soundKey) == allSounds.end()) {
        cout << "Sound not found: " << soundKey << endl;
        return;
    }

    Sound targetSound = allSounds[soundKey];

    SetSoundPan(targetSound,pan);
    SetSoundPitch(targetSound,pitch);
    SetSoundVolume(targetSound,volume * maxVolume);
    PlaySound(targetSound);
}

void SoundManager::PlaySound2DRandomized(string soundKey, float pitchRange, float volumeRange, float pan) {

    float pitch = 1.0f + GetRandomValue(-pitchRange * 100,pitchRange * 100) * 0.01f;
    float volume = 1.0f + GetRandomValue(-volumeRange * 100,volumeRange * 100) * 0.01f;

    PlaySound2D(soundKey,pitch,volume,pan);
}

//Don't use this too frequently for performance reasons
void SoundManager::PlaySound3D(string soundKey, Vector3 position,float maxDistance) {

    if(cam == nullptr) {
        cout << "Camera not set for sound manager!" << endl;
        return;
    }

    float distance = Vector3Distance(position,cam->position);
    float volume = 1.0f - (distance / maxDistance);


    Vector3 camRight = GetCameraRight(cam);
    Vector3 soundDir = Vector3Normalize(Vector3Subtract(position,cam->position));
    float dot = Vector3DotProduct(soundDir,camRight) * -1.0f;

    float pan = (dot + 1.0f) * 0.5f;

    PlaySound2D(soundKey,1.0f,volume,pan);

}

void SoundManager::PlaySoundLooped(string soundKey) {

    loopingSounds.push_back(soundKey);
}

void SoundManager::StopSoundLooped(string soundKey) {

    for (int i = 0; i < loopingSounds.size(); ++i) {
        if (loopingSounds[i] == soundKey) {
            loopingSounds.erase(loopingSounds.begin() + i);
            StopSound(allSounds[soundKey]);
            return;
        }
    }

}


void SoundManager::Update(float deltaTime,bool isGamePaused) {

    //Loop through looping sounds and play them
    for (auto& soundKey : loopingSounds) {

        if(isGamePaused) {
            StopSound(allSounds[soundKey]);
            continue;
        }

        //Constantly loop sound if it's not playing
        if(IsSoundPlaying(allSounds[soundKey]) == false)
            PlaySound2D(soundKey,1.0f,1.0f,0.5f);
    }

    if(IsKeyPressed(KEY_M))
    {
        maxVolume = maxVolume == 1.0f ? 0.0f : 1.0f;
        cout << "Max volume changed to " << maxVolume << endl;
    }

}

void SoundManager::Init(Camera3D* mainCamera) {
    cam = mainCamera;
}
