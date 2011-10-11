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

class EffectEvent;

//typedef boost::any (EffectEvent::*SetAttrFunction)(boost::any);

/*
typedef boost::function<void (int x)> IntAttrFunction;
typedef boost::function<void (float x)> FloatAttrFunction;
typedef boost::function<void (bool x)> BoolAttrFunction;
typedef boost::function<void (float x, float y, float z)> ColorAttrFunction;
typedef boost::function<void (string x)> TextureAttrFunction;
typedef boost::function<void (float x, float y, float z)> Vec3AttrFunction;
*/

struct EffectAttribute
{
    string name;
    string type;
    boost::any value;
    //SetAttrFunction setAttrFunc;
};

typedef boost::unordered_map<string, EffectAttribute> EffectAttrMap;

class EffectEvent : public BloomNode {

public:
    EffectEvent() :
        mEnabled(true),
        mExtension(""),
        mEmitterPosition(Vec3f( 0.0f, 0.0f, 0.0f ))
        {}
        
    ~EffectEvent();
    
    virtual void registerAttributes() {}
    string getPathExtension() { return mExtension; }
    
    virtual void setup() {}
    virtual void update(const ci::CameraPersp &camera) {}
    virtual void draw() {}
    
    void setTransform( const ci::Matrix44f &transform ) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    void registerAttribute(string attrName, string attrType);
    
    EffectAttrMap getAttributes() { return mAttributes; }
    void setEnabled(bool enabled) { mEnabled = enabled; }
    void setEmitterPosition(Vec3f position) { mEmitterPosition = position; }
    bool isEnabled() { return mEnabled; }
    
    EffectAttrMap mAttributes;
    
protected:

    virtual void processAttributes() {};
    
    gl::Texture getTexture(boost::any currentValue) 
    { 
        return gl::Texture( loadImage( loadResource( boost::any_cast<string>(currentValue) ) ) );
    }
    float getFloat(boost::any currentValue)
    {
        return boost::any_cast<float>(currentValue);
    }
    Color getColor(boost::any currentValue)
    {
        return boost::any_cast<Color>(currentValue);
    }
    Vec3f getVector3(boost::any currentValue)
    {
        return boost::any_cast<Vec3f>(currentValue);
    }
    int getInt(boost::any currentValue)
    {
        return boost::any_cast<int>(currentValue);
    }
    
    // TODOs:
    // mAttachment (parent or joint attachments)
    // bool mInheritTransform
    
    ci::Matrix44f mTransform;
    
    bool mEnabled;
    Vec3f mEmitterPosition;
    string mExtension;
};

typedef std::shared_ptr<EffectEvent> EffectEventRef;

