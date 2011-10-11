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

const boost::unordered_map<string, ChildEvent> CHILD_EVENTS = boost::assign::map_list_of
    ("ParticleEvent", PARTICLE_EVENT);

class Effect : public BloomNode {

public:
    Effect():
        mIsVisible(true)
    {}
    
    //TODO TEMP TESTING
    Effect(string effectPath);
    
    ~Effect();
    
    friend class EffectEvent;
    
    void setup();
    void update();
    void deepUpdate();
    void draw();
    
    void setCamera(ci::CameraPersp &camera);
    void setTransform( const ci::Matrix44f &transform ) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
private:
    json::Value getData(string effectPath);
    void parseAttr(const json::Value data, EffectAttribute &attr, EffectEvent *currentEvent);
    
    ci::Matrix44f mTransform;
    
    // TODO need to move this to manager later
    ci::CameraPersp* mCamera;
    bool mIsVisible;
    
    //TODO TEMP TESTING
    json::Value mData;
    
    //TODO need to convert to BloomNoderefs instead
    std::vector<EffectEvent *> mEvents;
    //typedef boost::unordered_map<string, EffectEvent*> ChildMap;
    //ChildMap mChildEvents;
};