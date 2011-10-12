//
//  EffectEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "BloomNode.h"

#include "cinder/Cinder.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include <boost/unordered_map.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <string>

#include "ciJson.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ci;
using namespace std;

class EffectAttribute
{
public:
    string mName;
    string mType;
    boost::any mValue;
    
    gl::Texture getTexture() 
    { 
        return gl::Texture( loadImage( loadResource( boost::any_cast<string>(mValue) ) ) );
    }
    float getFloat()
    {
        return boost::any_cast<float>(mValue);
    }
    Color getColor()
    {
        return boost::any_cast<Color>(mValue);
    }
    Vec3f getVector3()
    {
        return boost::any_cast<Vec3f>(mValue);
    }
    int getInt()
    {
        return boost::any_cast<int>(mValue);
    }
    Vec2f getVector2()
    {
        return boost::any_cast<Vec2f>(mValue);
    }
    string getString()
    {
        return boost::any_cast<string>(mValue);
    }
};

typedef boost::unordered_map<string, EffectAttribute> EffectAttrMap;

class EffectEvent : public BloomNode {

public:
    EffectEvent() :
        mEnabled(true),
        mFileExtension(""),
        mIsStarted(false),
        mIsStopping(false),
        mIsStopped(false),
        mHardStop(false),
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
    
    // TODO make one function and one state variable
    bool isStarted() { return mIsStarted; }
    bool isStopped() { return mIsStopped; }
    bool isStopping() { return mIsStopping; }
    
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
    
    //TODO make these all one variable (enum?)
    bool mIsStarted;
    bool mIsStopping;
    bool mIsStopped;
            
    bool mEnabled;
    Vec3f mEmitterPosition;
    Vec3f mEmitterOrientation;
    
    string mFileExtension;
};

typedef std::shared_ptr<EffectEvent> EffectEventRef;

