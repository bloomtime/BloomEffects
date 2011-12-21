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
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

typedef std::vector<ci::Vec3f> floatCurvePoints;
typedef ci::BSpline2f floatCurve;

struct AttributeCurvePoints
{ 
    std::vector<ci::Vec2f> valuePoints;
    std::vector<ci::Vec2f> variancePoints;
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
    
    std::string mName;
    AttributeType mType;
    boost::any mValue;
    
    // quick and dirty cache
    // TODO: use BloomUtils PreLoader instead of this
    static boost::unordered_map<std::string, ci::gl::Texture> sTextureCache;
    static boost::unordered_map<std::string, ci::gl::GlslProg> sShaderCache;
    
    ci::gl::Texture getTexture();
    ci::gl::GlslProg getShader();
    AttributeCurvePoints getCurvePoints();
    
    float getFloat()
    {
        return boost::any_cast<float>(mValue);
    }
    ci::Color getColor()
    {
        return boost::any_cast<ci::Color>(mValue);
    }
    ci::Vec3f getVector3()
    {
        return boost::any_cast<ci::Vec3f>(mValue);
    }
    int getInt()
    {
        return boost::any_cast<int>(mValue);
    }
    ci::Vec2f getVector2()
    {
        return boost::any_cast<ci::Vec2f>(mValue);
    }
    bool getBool()
    {
        return boost::any_cast<bool>(mValue);
    }
    std::string getString()
    {
        return boost::any_cast<std::string>(mValue);
    }
    
};