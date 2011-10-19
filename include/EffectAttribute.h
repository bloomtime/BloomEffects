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
#include "cinder/Rand.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

typedef vector<Vec3f> floatCurvePoints;
typedef vector<Vec2f> varianceCurvePoints;
typedef BSpline2f floatCurve;

class AttributeCurve
{
public:
    AttributeCurve() {}
    ~AttributeCurve() {}
    
    floatCurve valueCurve;
    varianceCurvePoints variancePoints;
    
    float getValueAtTime(float time)
    {
        float currentValue = valueCurve.getPosition(time)[1];
        return currentValue;
        
        // TODO need smooth variance
        //float currentVariance = variances.getPosition(time)[1];
        //return currentValue + Rand::randFloat(-currentVariance, currentVariance);
    }
};

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
    AttributeCurve getCurve()
    {
        vector<ci::Vec2f> valueCurveInput;
        
        floatCurvePoints curvePoints = boost::any_cast<floatCurvePoints>(mValue);
        
        // create begin and end points for tangents
        curvePoints.insert(curvePoints.begin(), curvePoints[0]);
        curvePoints.push_back(curvePoints[curvePoints.size()-1]);
        
        // insert begin point for variance (curve constructed later)
        varianceCurvePoints variancePoints;
        variancePoints.push_back(Vec2f(curvePoints[0][0], curvePoints[0][2]));

        Vec3f previousPoint = curvePoints[0];
        Vec3f currentPoint = curvePoints[1];
        
        for (int i=1; i < curvePoints.size(); i++)
        {            
            variancePoints.push_back(Vec2f(curvePoints[i][0], curvePoints[i][2]));
            
            //TODO catmull-rom, not sure if needed now
            //if (i == 0)
            //{
            //    console() << curvePoints[i] << "|" << std::endl;
            //    curveInput.push_back(curvePoints[i]);
            //    continue;
            //}
              
            Vec3f nextPoint = curvePoints[i+1];
            
            Vec2f currentVec = Vec2f(currentPoint[0], currentPoint[1]);
            Vec2f previousVec = Vec2f(previousPoint[0], previousPoint[1]);
            Vec2f nextVec = Vec2f(nextPoint[0], nextPoint[1]);
            
            
            Vec2f b1 = currentVec - (currentVec - previousVec) * TANGENT_LENGTH;
            Vec2f b2 = currentVec + (nextVec - currentVec) * TANGENT_LENGTH;
            
            Vec2f currentVar = Vec2f(currentPoint[0], currentPoint[2]);
            Vec2f previousVar = Vec2f(previousPoint[0], previousPoint[2]);
            Vec2f nextVar = Vec2f(nextPoint[0], nextPoint[2]);
            
            Vec2f vb1 = currentVar - (currentVar - previousVar) * TANGENT_LENGTH;
            Vec2f vb2 = currentVar + (nextVar - currentVar) * TANGENT_LENGTH;
            
            //console() << b1 << "|" << b2 << "|" << curvePoints[i] << std::endl;
            
            valueCurveInput.push_back(b1);
            valueCurveInput.push_back(currentVec);
            valueCurveInput.push_back(b2);
            
            currentPoint = nextPoint;
            previousPoint = currentPoint;
        }
        
        float last = curvePoints.size() - 1;
        variancePoints.push_back(Vec2f(curvePoints[last][0], curvePoints[last][2]));
        floatCurve newValueCurve = BSpline2f( valueCurveInput, 3, false, true );
        
        AttributeCurve newCurve;
        newCurve.valueCurve = newValueCurve;
        newCurve.variancePoints = variancePoints;
        
        //console() << newCurve.getPosition(0) << " yay" << std::endl;
        return newCurve;
    }
};