//
//  Effect.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"

#include "cinder/Camera.h"
#include "cinder/DataSource.h"
#include "cinder/Function.h"
#include "cinder/Matrix.h"
#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include <vector>
#include <list>

//TODO for now Effect is going to handle the file parsing and data grabbing.  Will need to separate this out when I figure out how the custom attributes for different EffectEvents are handled    
//#include "ciJson.h"
#include "json/json.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;


//TODO there might be a better way to register these types
enum ChildEvent {
    PARTICLE_EVENT
};

// add new child events here
const boost::unordered_map<string, ChildEvent> CHILD_EVENTS = boost::assign::map_list_of
    ("ParticleEvent", PARTICLE_EVENT);

class Effect {

public:
    Effect():
        mIsVisible(true),
        mIsStarted(false),
        mIsStopped(false),
        mStartedTime(-1.0f)
    {
        mTransform.setToIdentity();
    }
    
    //TODO TEMP TESTING
    Effect(string effectPath);
    
    ~Effect();
    
    friend class EffectEvent;
    
    void setup();
    void update();
    void deepUpdate();
    void draw();
    
    void start();
    void stop(bool hardStop=false);  
    float getEffectElapsedSeconds();
    
    void initializeData();
    void setCamera(ci::CameraPersp &camera);
    void setTransform( const ci::Matrix44f &transform ) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    bool isStarted() { return mIsStarted; }
    bool isStopped() { return mIsStopped; }

private:
    vector<Json::Value> readVector(Json::Value object, string key);
    Json::Value getData(string effectPath);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEvent *currentEvent);
    
    ci::Matrix44f mTransform;
    
    // TODO need to move this to effectsmanager later
    ci::CameraPersp* mCamera;
    bool mIsVisible;
    float mStartedTime;
    
    bool mIsStarted;
    bool mIsStopped;
    
    //TODO TEMP TESTING-parsing should be moved elsewhere
    Json::Value mData;
    
    //TODO maybe need to convert to BloomNoderefs instead
    std::list<EffectEvent *> mEvents;
};