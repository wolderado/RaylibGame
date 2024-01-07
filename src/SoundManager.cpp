//
// Created by Salih on 07-Jan-24.
//

#include "SoundManager.h"



// Initialize the static member variable
std::map<string,Sound> SoundManager::allSounds;


void SoundManager::LoadSounds() {

    cout << "Loading sounds..." << endl;

    // Open the directory
    string folderPath  = "resources/Sounds/";



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
                allSounds[fileName.substr(0, fileName.size() - 4)] = sound;

                std::cout << "Loaded sound: " << fileName << std::endl;
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
