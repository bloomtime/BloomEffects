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
#include "cinder/gl/GlslProg.h"
#include "cinder/app/App.h"
#include "cinder/BSpline.h"
#include "cinder/Rand.h"
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

typedef vector<Vec3f> floatCurvePoints;
typedef BSpline2f floatCurve;

struct AttributeCurvePoints
{ 
    vector<Vec2f> valuePoints;
    vector<Vec2f> variancePoints;
};

const float TANGENT_LENGTH = .2f; //.5f for catmull-rom 

enum AttributeType 
{
    ATTR_COLOR,
    ATTR_TEXTURE,
    ATTR_SHADER,
    ATTR_FLOAT,
    ATTR_VECTOR3,
    ATTR_VECTOR2,
    ATTR_BOOL,
    ATTR_STRING,
    ATTR_CURVE
};

class EffectAttribute
{
public:
    EffectAttribute() {}
    ~EffectAttribute() {}
    
    string mName;
    AttributeType mType;
    boost::any mValue;
    
    // quick and dirty cache
    static boost::unordered_map<string, gl::Texture> sTextureCache;
    static boost::unordered_map<string, gl::GlslProg> sShaderCache;
    
    gl::Texture getTexture() 
    { 
        gl::Texture::Format mipFmt;
        mipFmt.enableMipmapping( true );
        mipFmt.setMinFilter( GL_LINEAR_MIPMAP_NEAREST );    
        mipFmt.setMagFilter( GL_LINEAR ); // TODO: experiment with GL_NEAREST where appropriate
        
        string val = boost::any_cast<string>(mValue);
        
        if (sTextureCache.find(val) != sTextureCache.end()) {
            return sTextureCache[val];
        }
        
        gl::Texture texture = gl::Texture( loadImage( loadResource( val ) ), mipFmt );
        sTextureCache[val] = texture;
        return texture;
    }
    gl::GlslProg getShader()
    {
        string prefix = boost::any_cast<string>(mValue);
        string vert = prefix + "_vert.glsl";
        string frag = prefix + "_frag.glsl";

        if (sShaderCache.find(prefix) != sShaderCache.end()) {
            return sShaderCache[prefix];
        }
        
        gl::GlslProg prog = gl::GlslProg( loadResource( vert ), loadResource( frag ) );
        sShaderCache[prefix] = prog;
        return prog;
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
    AttributeCurvePoints getCurvePoints()
    {
        vector<ci::Vec2f> valueCurveInput;
        
        floatCurvePoints curvePoints = boost::any_cast<floatCurvePoints>(mValue);
        
        // insert begin point for values and variance (baked curve constructed later)
        vector<Vec2f> valuePoints;
        vector<Vec2f> variancePoints;
        
        valuePoints.push_back(Vec2f(curvePoints[0][0], curvePoints[0][1]));
        variancePoints.push_back(Vec2f(curvePoints[0][0], curvePoints[0][2]));

        Vec3f previousPoint = curvePoints[0];
        Vec3f currentPoint = curvePoints[1];
        
        for (int i=0; i < curvePoints.size(); i++)
        {            
            valuePoints.push_back(Vec2f(curvePoints[i][0], curvePoints[i][1]));
            variancePoints.push_back(Vec2f(curvePoints[i][0], curvePoints[i][2]));
        }

        // insert dupe end points for values and variance
        float last = curvePoints.size() - 1;
        valuePoints.push_back(Vec2f(curvePoints[last][0], curvePoints[last][1]));
        variancePoints.push_back(Vec2f(curvePoints[last][0], curvePoints[last][2]));
            
        AttributeCurvePoints newCurvePoints;
        newCurvePoints.valuePoints = valuePoints;
        newCurvePoints.variancePoints = variancePoints;
        
        return newCurvePoints;
    }
};