//
//  EffectEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "BloomNode.h"
#include "EffectAttribute.h"

#include "cinder/Cinder.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include <boost/unordered_map.hpp>
#include <string>

#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"
#include <boost/unordered_map.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

enum EventState 
{
    EVENT_INITIALIZED,
    EVENT_STARTED,
    EVENT_RUNNING,
    EVENT_STOPPING,
    EVENT_STOPPED
};

typedef boost::unordered_map<string, EffectAttribute> EffectAttrMap;

class EffectEvent : public BloomNode {

public:
    EffectEvent() :
        mEnabled(true),
        mFileExtension(""),
        mEventState(EVENT_INITIALIZED),
        mStartTime(-1.0f),
        mLifetime(0.0f),
        mEmitterPosition(Vec3f( 0.0f, 0.0f, 0.0f )),
        mEmitterOrientation(Vec3f( 0.0f, 0.0f, 0.0f))
        {}
        
    ~EffectEvent();
    
    virtual void registerAttributes() {}
    string getPathExtension() { return mFileExtension; }
    
    virtual void setup() {}
    virtual void update(const ci::CameraPersp &camera) {}
    virtual void draw() {}
    
    void setTransform( const ci::Matrix44f &transform ) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    void registerAttribute(string attrName, string attrType);
        
    EffectAttrMap getAttributes() { return mAttributes; }
    void setAttribute(string name, boost::any value) { mAttributes[name].mValue = value; }
    void setEnabled(bool enabled) { mEnabled = enabled; }
    void setStartTime(float startTime) { mStartTime = startTime; }
    float getStartTime() { return mStartTime; }
    void setEmitterPosition(Vec3f position) { mEmitterPosition = position; }
    void setEmitterOrientation(Vec3f orientation) { mEmitterOrientation = orientation; }
    bool isEnabled() { return mEnabled; }
    
    // Event state checks
    bool isInitialized()  { return mEventState == EVENT_INITIALIZED; }
    bool isStarted()      { return mEventState == EVENT_STARTED;     }
    bool isRunning()      { return mEventState == EVENT_RUNNING;     }
    bool isStopping()     { return mEventState == EVENT_STOPPING;    }
    bool isStopped()      { return mEventState == EVENT_STOPPED;     }
    
    void start();
    void stop(bool hardStop=false);
    float getEventElapsedSeconds();
    
    EffectAttrMap mAttributes;
    
protected:

    virtual void processAttributes() {};
    
    // TODOs:
    // mAttachment (parent or joint attachments)
    // bool mInheritTransform
    
    ci::Matrix44f mTransform;
    float mLifetime;
    float mStartTime;
    bool mHardStop;
    
    EventState mEventState;
            
    bool mEnabled;
    Vec3f mEmitterPosition;
    Vec3f mEmitterOrientation;
    
    string mFileExtension;
};

typedef std::shared_ptr<EffectEvent> EffectEventRef;

