//
//  AudioManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/Cinder.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <string>

using namespace std;

class AudioManager;

typedef std::shared_ptr<class AudioManager> AudioManagerRef;
//typedef std::shared_ptr<FMOD::Sound> SoundRef;

class AudioManager {
public:
    static AudioManagerRef create()
    { 
        return AudioManagerRef(new AudioManager());
    }
    
    ~AudioManager();
    
    void setup();
    
    FMOD::System   *mSystem;
    FMOD::Channel  *mChannel1;
        
    void ERRCHECK(FMOD_RESULT result);

    FMOD::Sound* createSound(string filepath);
    void playSound(FMOD::Sound* sound);
    bool isPlaying(FMOD::Sound* sound);
    
private:

    AudioManager();
};
