//
//  EffectEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectAttribute.h"
#include "RenderManager.h"

#include "cinder/Cinder.h"
#include "cinder/Camera.h"
#include "cinder/Camera.h"
#include "cinder/Timer.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include <boost/unordered_map.hpp>
#include <string>

//#include "EffectsRenderer.h"

enum EventState 
{
    EVENT_INITIALIZED,
    EVENT_STARTED,
    EVENT_RUNNING,
    EVENT_STOPPING,
    EVENT_STOPPED
};

typedef boost::unordered_map<std::string, EffectAttribute> EffectAttrMap;

const boost::unordered_map<std::string, RenderLayer> RENDER_LAYERS = boost::assign::map_list_of
    ("BACKGROUND", LAYER_BACKGROUND)
    ("SCENE", LAYER_SCENE)
    ("EFFECTS", LAYER_EFFECTS)
    ("FOREGROUND", LAYER_FOREGROUND)
    ("PREPOST", LAYER_PREPOST)
    ("POST", LAYER_POST)
    ("UI", LAYER_UI);
    
class EffectEvent;

//typedef std::shared_ptr<EffectsRenderer> EffectRendererRef;
typedef std::shared_ptr<EffectEvent> EffectEventRef;
typedef std::shared_ptr<ci::CameraPersp> CameraRef;

const float ELAPSED_MAX = .5f;

class EffectEvent {

public:

    static EffectEventRef create();
        
    ~EffectEvent();
    
    virtual void registerAttributes() {}
    std::string getPathExtension() { return mFileExtension; }
    
    virtual void setup(ci::Vec2f windowSize) {}
    virtual void update() {}
    virtual void draw(bool enabled=true) {}
    
    void setRenderManager(RenderManagerRef renderMgr) { mRenderManager = renderMgr; }
    void setCamera(CameraRef camera) { mCamera = camera; }
    void setKeyLightDir(ci::Vec3f dir) { mKeyLightDir = dir.normalized(); }
    
    void setParentTransform( const ci::Matrix44f &transform ) 
    { 
        mParentTransform = transform; /* copy OK */ 
        mParentTransformChanged = true;
    }
    
    virtual void setTintColor( ci::Vec3f color ) {}
    virtual void setTexture( ci::gl::Texture texture, int ID) {}
    
    ci::Matrix44f getTransform() const { return mParentTransform; /* copy OK */ }
    
    void registerAttribute(std::string attrName, AttributeType attrType);
        
    EffectAttrMap getAttributes() { return mAttributes; }
    EffectAttribute getAttribute(std::string attrName);
    
    void setAttributes(EffectAttrMap attributes) {  mAttributes = attributes; }
    void setAttribute(std::string name, boost::any value) { mAttributes[name].mValue = value; }
    
    void setEnabled(bool enabled) { mEnabled = enabled; }
    void setStartTime(float startTime) { mStartTime = startTime; }
    float getStartTime() { return mStartTime; }
    
    void setSourcePosition(ci::Vec3f position) 
    { 
        mLocalPosition = position;
        mSourcePosition = mParentTransform.transformPoint(position);
    }
    
    void setSourceOrientation(ci::Vec3f orientation) 
    { 
        mLocalOrientation.set(ci::toRadians(orientation[0]),
                              ci::toRadians(orientation[1]), 
                              ci::toRadians(orientation[2]));
        mSourceOrientation = mLocalOrientation * ci::Quatf(mParentTransform); 
    }
    
    void setSourceScale(float scale)
    {
        mSourceScale = scale;
    }
    
    void updateSource();
    void setInitialized() { mEventState = EVENT_INITIALIZED; }
    
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
        mHardStop(false),
        mLocalPosition(ci::Vec3f( 0.0f, 0.0f, 0.0f )),
        mSourcePosition(ci::Vec3f( 0.0f, 0.0f, 0.0f )),
        mSourceOrientation(ci::Quatf::identity()),
        mParentTransformChanged(false),
        mSourceScale(1.0f),
        mPreviousElapsed(0.0f),
        mActualSeconds(0.0f),
        mKeyLightDir(ci::Vec3f(0.0f, -1.0f, 0.0f)),
        mTimer(true) // true = auto-start
    {
        mParentTransform.setToIdentity();
    }
    
    virtual void processAttributes() {};
    
    CameraRef mCamera;
    RenderManagerRef mRenderManager;
    
    double mActualSeconds;
    double mPreviousElapsed;
    
    ci::Vec3f mKeyLightDir;
    
    //parent transform
    ci::Matrix44f mParentTransform;
    float mLifetime;
    float mStartTime;
    bool mHardStop;
    bool mInheritTransform;
    
    EventState mEventState;
        
    bool mEnabled;
    bool mParentTransformChanged;
    ci::Vec3f mLocalPosition;
    ci::Vec3f mSourcePosition;
    float mSourceScale;
    
    ci::Quatf mLocalOrientation;
    ci::Quatf mSourceOrientation;
    
    std::string mFileExtension;
    
    ci::Timer mTimer;
    
    template<typename T> ci::CallbackId mCallbackId;
};
