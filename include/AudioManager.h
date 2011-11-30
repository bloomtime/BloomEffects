//
//  AudioManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"

#include "fmod.hpp"
#include "fmod_errors.h"
#include <string>

using namespace std;
using namespace ci;

class AudioManager;

typedef std::shared_ptr<class AudioManager> AudioManagerRef;
//typedef std::shared_ptr<FMOD::Sound> SoundRef;
typedef std::shared_ptr<CameraPersp> CameraRef;

const int MAX_CHANNELS = 4;

class AudioManager {
public:
    static AudioManagerRef create()
    { 
        return AudioManagerRef(new AudioManager());
    }
    
    ~AudioManager();
    
    void setup();
    void update();
    
    // for 3D sounds
    void setCamera(CameraRef camera) { mCamera = camera; }
    
    FMOD::System   *mSystem;
    FMOD::Channel  *mChannels[MAX_CHANNELS];
            
    void ERRCHECK(FMOD_RESULT result);

    FMOD::Sound* createSound(string filepath, bool looping = false);
    void playSound(FMOD::Sound* sound, float volume=1.0f, Vec3f pos = Vec3f(0.0f, 0.0f, 0.0f), Vec3f vel = Vec3f(0.0f, 0.0f, 0.0f));
    bool isPlaying(FMOD::Sound* sound);
    void stopSound(FMOD::Sound* sound);
    void setVolume(FMOD::Sound* sound, float volume);
    void set3DSound(bool is3d) { mIs3D = is3d; }
    
protected:
    void updateListenerPosition();
    
    bool mIs3D;
    
private:
    CameraRef mCamera;
    
    AudioManager();
};
