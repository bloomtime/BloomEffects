//
//  Effect.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "BloomNode.h"
#include "EffectEvent.h"

#include "cinder/Camera.h"
#include "cinder/DataSource.h"
#include "cinder/Function.h"
#include "cinder/Matrix.h"
#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include <vector>

//TODO for now Effect is going to handle the file parsing and data grabbing.  Will need to separate this out when I figure out how the custom attributes for different EffectEvents are handled    
#include "ciJson.h"
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

class Effect : public BloomNode {

public:
    Effect():
        mIsVisible(true),
        mIsStarted(false),
        mIsStopped(false),
        mStartedTime(-1.0f)
    {}
    
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
    json::Value getData(string effectPath);
    void parseAttr(const json::Value data, EffectAttribute &attr, EffectEvent *currentEvent);
    
    ci::Matrix44f mTransform;
    
    // TODO need to move this to effectsmanager later
    ci::CameraPersp* mCamera;
    bool mIsVisible;
    bool mIsStarted;
    bool mIsStopped;
    float mStartedTime;
    
    //TODO TEMP TESTING-parsing should be moved elsewhere
    json::Value mData;
    
    //TODO maybe need to convert to BloomNoderefs instead
    std::list<EffectEvent *> mEvents;
};