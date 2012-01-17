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
#include "fmod_event.hpp"
#include <string>

namespace bloom { namespace effects {

class AudioManager;

typedef std::shared_ptr<class AudioManager> AudioManagerRef;
//typedef std::shared_ptr<FMOD::Sound> SoundRef;
typedef std::shared_ptr<ci::CameraPersp> CameraRef;

const int MAX_CHANNELS = 32;

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
    
    FMOD::EventSystem *mEventSystem;
    
    void ERRCHECK(FMOD_RESULT result);

    void loadFEV(std::string filepath);
       
    FMOD::Event* createEvent(std::string eventPath);
    void playEvent(FMOD::Event* event, float volume=1.0f, ci::Vec3f pos = ci::Vec3f(0.0f, 0.0f, 0.0f), ci::Vec3f vel = ci::Vec3f(0.0f, 0.0f, 0.0f));
    bool isEventPlaying(FMOD::Event* event);
    
    // below is fmod ex stuff
    FMOD::Sound* createSound(std::string filepath, bool looping = false);
    void playSound(FMOD::Sound* sound, float volume=1.0f, ci::Vec3f pos = ci::Vec3f(0.0f, 0.0f, 0.0f), ci::Vec3f vel = ci::Vec3f(0.0f, 0.0f, 0.0f));
    bool isSoundPlaying(FMOD::Sound* sound);
    void stopSound(FMOD::Sound* sound);
    void setSoundVolume(FMOD::Sound* sound, float volume);
    void set3DSound(bool is3d) { mIs3D = is3d; }
    
protected:
    void updateListenerPosition();
    
    bool mIs3D;
    
private:
    CameraRef mCamera;
    
    AudioManager();
};

}} // namespace bloom::effects
