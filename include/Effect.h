//
//  Effect.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
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
    PARTICLE_EVENT,
    POST_EVENT,
    SOUND_EVENT
};

// add new child events here
const boost::unordered_map<string, ChildEventType> CHILD_EVENT_TYPES = boost::assign::map_list_of<string, ChildEventType>
    ("ParticleEvent", PARTICLE_EVENT)
    ("PostEvent",     POST_EVENT)
    ("SoundEvent",    SOUND_EVENT);

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
    
    void start();
    void stop(bool hardStop=false);  
    double getEffectElapsedSeconds();
    
    void initializeData(Json::Value data);
    void setCamera(CameraRef camera);
    
    void setTransform( const ci::Matrix44f &transform );
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    bool isStarted() { return mIsStarted; }
    bool isStopped() { return mIsStopped; }

protected:

    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
    double mActualSeconds;
    double mPreviousElapsed;
    
    ci::Matrix44f mTransform;
    bool mIsVisible;
    float mStartedTime;
    
    CameraRef mCamera;
    
    bool mIsStarted;
    bool mIsStopped;

    std::list<EffectEventRef> mEvents;
    
    ci::Timer mTimer;
    
private:  

    Effect():
        mIsVisible(true),
        mIsStarted(false),
        mIsStopped(false),
        mStartedTime(-1.0f),
        mTimer(true) // true = auto-start
    {
        mTransform.setToIdentity();
    }
};