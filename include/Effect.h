//
//  Effect.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
#include "ParticleEvent.h"

#include "cinder/Camera.h"
#include "cinder/Matrix.h"
#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include <vector>
#include <list>

#include "json/json.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class Effect;

// for sharing ownership:
typedef std::shared_ptr<Effect> EffectRef;

// for avoiding circular refs:
typedef std::weak_ptr<Effect> EffectWeakRef;

//TODO there might be a better way to register these types
enum ChildEvent {
    PARTICLE_EVENT
};

// add new child events here
const boost::unordered_map<string, ChildEvent> CHILD_EVENTS = boost::assign::map_list_of
    ("ParticleEvent", PARTICLE_EVENT);

class Effect {

public:

    static EffectRef create();
    static EffectRef createFromPath(string path);
    
    ~Effect();
    
    friend class EffectEvent;
    
    void setup();
    void update();
    void draw();
    
    void setTintColor(Vec3f color);
    void setSourceScale(float scale);
    
    void start();
    void stop(bool hardStop=false);  
    float getEffectElapsedSeconds();
    
    void initializeData();
    void setCamera(ci::CameraPersp *camera);
    void setTransform( const ci::Matrix44f &transform );
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    bool isStarted() { return mIsStarted; }
    bool isStopped() { return mIsStopped; }

protected:

    vector<Json::Value> readVector(Json::Value object, string key);
    Json::Value getData(string effectPath);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
    ci::Matrix44f mTransform;
    bool mIsVisible;
    float mStartedTime;
    
    ci::CameraPersp* mCamera;
    
    bool mIsStarted;
    bool mIsStopped;
    
    //TODO TEMP TESTING-parsing should be moved elsewhere
    Json::Value mData;
    
    std::list<EffectEventRef> mEvents;
    
private:  

    Effect():
        mIsVisible(true),
        mIsStarted(false),
        mIsStopped(false),
        mStartedTime(-1.0f),
        mCamera(NULL)
    {
        mTransform.setToIdentity();
    }
    
    //TODO need to move the parsing elsewhere
    Effect(string effectPath);
};