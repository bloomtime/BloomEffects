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
#include "cinder/BSpline.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ci;
using namespace std;

typedef vector<Vec2f> floatCurvePoints;

//TEMP - needs to be bspline
typedef BSpline2f floatCurve;

const float CURVE_TENSION = .5f;  //experiment

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
    floatCurve getCurve()
    {
        floatCurve newCurve;
        /*
        floatCurvePoints curvePoints = boost::any_cast<floatCurvePoints>(mValue);
        vector<ci::Vec2f> curveInput;
        
        if (curvePoints.size() == 1)
            curvePoints.push_back(curvePoints[0]);
        
        Vec2f pFirst = curvePoints[0] + curvePoints[0] - curvePoints[1];
        curvePoints.insert (curvePoints.begin(), pFirst);
        
        float last = curvePoints.size() - 2;
        Vec2f pLast = curvePoints[last] + curvePoints[last] - curvePoints[last-1];
        curvePoints.push_back(pLast);

        for (int i=0; i < curvePoints.size()-1; i++)
        {
            Vec2f currentPoint = curvePoints[i];
            
            if (i == 0)
            {
                curveInput.push_back(currentPoint);
                continue;
            }
            
            console() << curvePoints[i+3][0] << ","<< curvePoints[i+3][1] << "|" << std::endl;
            
            // TODO need to handle end point
            Vec2f b1 = curvePoints[i-1] + (curvePoints[i] - curvePoints[i-1]) * CURVE_TENSION;
            Vec2f b2 = curvePoints[i+2] - (curvePoints[i+1] - curvePoints[i]) * CURVE_TENSION;
            
            curveInput.push_back(curvePoints[i]);
        }
        */
        
        return newCurve;
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

