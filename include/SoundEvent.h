//
//  SoundEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "AudioManager.h"
#include "EffectEvent.h"

#include "cinder/Rand.h"
#include "cinder/gl/GlslProg.h"
#include <vector>
#include <list>

using namespace ci;
using namespace std;

class SoundEvent;

typedef std::shared_ptr<SoundEvent> SoundEventRef;

class SoundEvent : public EffectEvent {

public:

    static SoundEventRef create(AudioManagerRef audioMgr);

    ~SoundEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type)
        registerAttribute("Path",             ATTR_STRING);
    }
    
    void setup();
    void update();
    void draw();
    
protected:
    //-------------------------------------
    
    void processAttributes();

    string mFilePath;
    AudioManagerRef mAudioManager;
    FMOD::Sound* mSound;
            
private: 

    SoundEvent(AudioManagerRef audioMgr);
};