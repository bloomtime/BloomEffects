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

enum PlayMode {
    PLAY_ONCE,
    PLAY_LOOPING
};

// add new Emit Modes here
const boost::unordered_map<string, PlayMode> PLAY_MODES = boost::assign::map_list_of
    ("Once", PLAY_ONCE)
    ("Looping", PLAY_LOOPING);

class SoundEvent;

typedef std::shared_ptr<SoundEvent> SoundEventRef;

class SoundEvent : public EffectEvent {

public:

    static SoundEventRef create(AudioManagerRef audioMgr);

    ~SoundEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type)
        registerAttribute("Path",          ATTR_STRING);
        registerAttribute("Lifetime",      ATTR_FLOAT);  // when 0, it just plays until stops, or forever if looping
        registerAttribute("FadeTime",      ATTR_FLOAT);  // when looping, amount of time it takes to fade out when stopping
        registerAttribute("PlayMode",      ATTR_STRING);
        registerAttribute("Volume",        ATTR_FLOAT);
    }
    
    void setup();
    void update();
    void draw();
    
protected:
    //-------------------------------------
    
    void processAttributes();
    void doFade(float elapsed, float lifetime);

    string mFilePath;
    float mFadeTime;
    PlayMode mPlayMode;
    float mVolume;
    
    AudioManagerRef mAudioManager;
    FMOD::Sound* mSound;
            
private: 

    SoundEvent(AudioManagerRef audioMgr);
};