//
//  Effect.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
#include "FastParticleEvent.h"
#include "ParticleEvent.h"
#include "PostEvent.h"
#include "SoundEvent.h"

#include "cinder/Camera.h"
#include "cinder/Matrix.h"
#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Timer.h"
#include <vector>
#include <list>
#include <string>

#include "EffectJson.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

class Effect;

// for sharing ownership:
typedef std::shared_ptr<Effect> EffectRef;

// for avoiding circular refs:
typedef std::weak_ptr<Effect> EffectWeakRef;

typedef list<EffectEventRef> EffectEventList;

//TODO there might be a better way to register these types
enum ChildEventType {
    FASTPARTICLE_EVENT,
    PARTICLE_EVENT,
    POST_EVENT,
    SOUND_EVENT
};

// add new child events here
const boost::unordered_map<string, ChildEventType> CHILD_EVENT_TYPES = boost::assign::map_list_of<string, ChildEventType>
    ("FastParticleEvent", FASTPARTICLE_EVENT)
    ("ParticleEvent", PARTICLE_EVENT)
    ("PostEvent",     POST_EVENT)
    ("SoundEvent",    SOUND_EVENT);
    
enum EffectState 
{
    EFFECT_STARTED,
    EFFECT_RUNNING,
    EFFECT_STOPPED
};

class Effect {

public:

    static EffectRef create();
    
    ~Effect();
    
    friend class EffectEvent;
    
    void setup(EffectEventList events, ci::Vec2f windowSize);
    void update();
    void draw();
    
    void setTintColor(ci::Vec3f color);
    void setSourceScale(float scale);
    void setTexture(ci::gl::Texture texture, int ID=0);
    
    void start();
    void stop(bool hardStop=false);  
    double getEffectElapsedSeconds();
    
    void initializeData(Json::Value data);
    void setCamera(CameraRef camera);
    
    void setTransform( const ci::Matrix44f &transform );
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    bool isStarted() { return mEffectState == EFFECT_STARTED; }
    bool isRunning() { return mEffectState == EFFECT_RUNNING; }
    bool isStopped() { return mEffectState == EFFECT_STOPPED; }

protected:

    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
    double mActualSeconds;
    double mPreviousElapsed;
    
    ci::Matrix44f mTransform;
    bool mIsVisible;
    float mStartedTime;
    bool mIsChildrenRunning;
    
    CameraRef mCamera;
    
    EffectState mEffectState;

    std::list<EffectEventRef> mEvents;
    
    ci::Timer mTimer;
    
private:  

    Effect():
        mIsVisible(true),
        mEffectState(EFFECT_STOPPED),
        mStartedTime(-1.0f),
        mIsChildrenRunning(false),
        mTimer(true) // true = auto-start
    {
        mTransform.setToIdentity();
    }
};