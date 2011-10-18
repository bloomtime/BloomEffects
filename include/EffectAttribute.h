//
//  EffectEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <string>

#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"
#include "cinder/BSpline.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

typedef vector<Vec2f> floatCurvePoints;

//TEMP - needs to be bspline
typedef BSpline2f floatCurve;

const float TANGENT_LENGTH = .2f; //.5f for catmull-rom 

class EffectAttribute
{
public:
    EffectAttribute() {}
    ~EffectAttribute() {}
    
    string mName;
    string mType;
    boost::any mValue;
    
    gl::Texture getTexture() 
    { 
        gl::Texture::Format mipFmt;
        mipFmt.enableMipmapping( true );
        mipFmt.setMinFilter( GL_LINEAR_MIPMAP_NEAREST );    
        mipFmt.setMagFilter( GL_LINEAR ); // TODO: experiment with GL_NEAREST where appropriate
        
        return gl::Texture( loadImage( loadResource( boost::any_cast<string>(mValue) ) ), mipFmt );
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
    bool getBool()
    {
        return boost::any_cast<bool>(mValue);
    }
    string getString()
    {
        return boost::any_cast<string>(mValue);
    }
    floatCurve getCurve()
    {
        vector<ci::Vec2f> curveInput;
        
        floatCurvePoints curvePoints = boost::any_cast<floatCurvePoints>(mValue);
        
        if (curvePoints.size() == 1)
            curvePoints.push_back(curvePoints[0]);
        
        // TODO -- Doesn't handle non-flat tangents right now.  Will have to figure it out!
        /*
        Vec2f pFirst = curvePoints[0] + curvePoints[0] - curvePoints[1];
        curvePoints.insert (curvePoints.begin(), pFirst);
        
        float last = curvePoints.size() - 1;
        Vec2f pLast = curvePoints[last] + curvePoints[last] - curvePoints[last-1];
        curvePoints.push_back(pLast);
        */
        
        for (int i=0; i < curvePoints.size(); i++)
        {            
            //TODO catmull-rom
            //if (i == 0)
            //{
            //    console() << curvePoints[i] << "|" << std::endl;
            //    curveInput.push_back(curvePoints[i]);
            //    continue;
            //}
            
            Vec2f b1 = Vec2f(curvePoints[i][0] - (curvePoints[i][0] - curvePoints[i-1][0]) * TANGENT_LENGTH, curvePoints[i][1]);
            Vec2f b2 = Vec2f(curvePoints[i][0]  + (curvePoints[i+1][0] - curvePoints[i][0]) * TANGENT_LENGTH, curvePoints[i][1]);
            
            //console() << b1 << "|" << b2 << "|" << curvePoints[i] << std::endl;
            
            curveInput.push_back(b1);
            curveInput.push_back(curvePoints[i]);
            curveInput.push_back(b2);
        }
        
        floatCurve newCurve = BSpline2f( curveInput, 3, false, true );
        
        //console() << newCurve.getPosition(0) << " yay" << std::endl;
        return newCurve;
    }
};