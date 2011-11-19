//
//  EffectEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectAttribute.h"
#include "EffectsState.h"

#include "cinder/Cinder.h"
#include "cinder/Camera.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include <boost/unordered_map.hpp>
#include <string>

#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"
#include <boost/unordered_map.hpp>

//#include "EffectsRenderer.h"

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

class EffectEvent;

//typedef std::shared_ptr<EffectsRenderer> EffectRendererRef;
typedef std::shared_ptr<EffectEvent> EffectEventRef;
typedef std::shared_ptr<CameraPersp> CameraRef;

const float ELAPSED_MAX = .5f;

class EffectEvent {

public:

    static EffectEventRef create();
        
    ~EffectEvent();
    
    virtual void registerAttributes() {}
    string getPathExtension() { return mFileExtension; }
    
    virtual void setup() {}
    virtual void update() {}
    virtual void draw() {}
    
    void setCamera(CameraRef camera) { mCamera = camera; }
    void setState(EffectsStateRef state) { mState = state; }
    
    void setParentTransform( const ci::Matrix44f &transform ) 
    { 
        mParentTransform = transform; /* copy OK */ 
        mParentTransformChanged = true;
    }
    
    virtual void setTintColor( Vec3f color ) {}
    
    ci::Matrix44f getTransform() const { return mParentTransform; /* copy OK */ }
    
    void registerAttribute(string attrName, AttributeType attrType);
        
    EffectAttrMap getAttributes() { return mAttributes; }
    EffectAttribute getAttribute(string attrName);
    
    void setAttributes(EffectAttrMap attributes) {  mAttributes = attributes; }
    void setAttribute(string name, boost::any value) { mAttributes[name].mValue = value; }
    
    void setEnabled(bool enabled) { mEnabled = enabled; }
    void setStartTime(float startTime) { mStartTime = startTime; }
    float getStartTime() { return mStartTime; }
    
    void setSourcePosition(Vec3f position) 
    { 
        mLocalPosition = position;
        mSourcePosition = mParentTransform.transformPoint(position);
    }
    void setSourceOrientation(Vec3f orientation) 
    { 
        mLocalOrientation.set(toRadians(orientation[0]),toRadians(orientation[1]), toRadians(orientation[2]));
        mSourceOrientation = mLocalOrientation * Quatf(mParentTransform); 
    }
    void setSourceScale(float scale)
    {
        mSourceScale = scale;
    }
    
    void updateSource();
    
    bool isEnabled() { return mEnabled; }
    
    // Event state checks
    bool isInitialized()  { return mEventState == EVENT_INITIALIZED; }
    bool isStarted()      { return mEventState == EVENT_STARTED;     }
    bool isRunning()      { return mEventState == EVENT_RUNNING;     }
    bool isStopping()     { return mEventState == EVENT_STOPPING;    }
    bool isStopped()      { return mEventState == EVENT_STOPPED;     }
    
    void start();
    void stop(bool hardStop=false);
    double getEventElapsedSeconds();
    
    EffectAttrMap mAttributes;
    
protected:

    EffectEvent() :
        mEnabled(true),
        mFileExtension(""),
        mInheritTransform(false),
        mEventState(EVENT_INITIALIZED),
        mStartTime(-1.0f),
        mLifetime(0.0f),
        mLocalPosition(Vec3f( 0.0f, 0.0f, 0.0f )),
        mSourcePosition(Vec3f( 0.0f, 0.0f, 0.0f )),
        mSourceOrientation(Quatf::identity()),
        mParentTransformChanged(false),
        mSourceScale(1.0f),
        mPreviousElapsed(0.0f),
        mActualSeconds(0.0f)
    {
        mParentTransform.setToIdentity();
    }
    
    virtual void processAttributes() {};
    
    CameraRef mCamera;
    EffectsStateRef mState;
    
    double mActualSeconds;
    double mPreviousElapsed;
    
    //parent transform
    ci::Matrix44f mParentTransform;
    float mLifetime;
    float mStartTime;
    bool mHardStop;
    bool mInheritTransform;
    
    EventState mEventState;
            
    bool mEnabled;
    bool mParentTransformChanged;
    Vec3f mLocalPosition;
    Vec3f mSourcePosition;
    float mSourceScale;
    
    Quatf mLocalOrientation;
    Quatf mSourceOrientation;
    
    string mFileExtension;
    
};
