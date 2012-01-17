//
//  EffectAttribute.cpp
//  CinderFizz
//
//  Created by Tom Carden on 11/3/11.
//  Copyright (c) 2011 Bloom Studio, Inc. All rights reserved.
//

#include "EffectAttribute.h"

namespace bloom { namespace effects {

using namespace std;
using namespace ci;

// init static variables
boost::unordered_map<string, gl::Texture> EffectAttribute::sTextureCache;
boost::unordered_map<string, gl::GlslProg> EffectAttribute::sShaderCache;

gl::Texture EffectAttribute::getTexture() 
{     
    string val = boost::any_cast<string>(mValue);
    
    if (sTextureCache.find(val) != sTextureCache.end()) {
        return sTextureCache[val];
    }
  
    gl::Texture::Format mipFmt;
    mipFmt.enableMipmapping( true );
    mipFmt.setMinFilter( GL_LINEAR_MIPMAP_NEAREST );    
    mipFmt.setMagFilter( GL_LINEAR ); // TODO: experiment with GL_NEAREST where appropriate
    mipFmt.setWrap(GL_REPEAT, GL_REPEAT);
    
    gl::Texture texture = gl::Texture( loadImage( app::loadResource( val ) ), mipFmt );
    sTextureCache[val] = texture;
    return texture;
}

gl::GlslProg EffectAttribute::getShader()
{
    string prefix = boost::any_cast<string>(mValue);
    string vert = prefix + "_vert.glsl";
    string frag = prefix + "_frag.glsl";
    
    if (sShaderCache.find(prefix) != sShaderCache.end()) {
        return sShaderCache[prefix];
    }
    
    gl::GlslProg prog = gl::GlslProg( app::loadResource( vert ), app::loadResource( frag ) );
    sShaderCache[prefix] = prog;
    return prog;
}

AttributeCurvePoints EffectAttribute::getCurvePoints()
{
    vector<Vec2f> valueCurveInput;
    
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

}} // namespace bloom::effects
