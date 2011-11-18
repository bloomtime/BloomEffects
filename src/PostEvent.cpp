//
//  PostEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "PostEvent.h"
#include "EffectsRenderer.h"
// may want texture input later
//#include "cinder/gl/Texture.h"
//#include "cinder/gl/gl.h"

using namespace ci;

const string PATH_EXTENSION = ".post.json";

PostEventRef PostEvent::create()
{
    return PostEventRef( new PostEvent() );
}

PostEvent::PostEvent():
    mFadeTime(Vec2f(0.0f, 0.0f)),
    mShader("defaultPost")
{ 
    mLifetime = 1.0f;
    mFileExtension = PATH_EXTENSION;
    registerAttributes(); 
}
    
PostEvent::~PostEvent()
{
}

void PostEvent::processAttributes()
{
    // if can associate these member vars with the attr name that would be sweet
    mLifetime = mAttributes.at("Lifetime").getFloat();
    mFadeTime = mAttributes.at("FadeTime").getVector2();
    mShader = mAttributes.at("Shader").getString();
}
void PostEvent::setup()
{ 
    processAttributes();
    
    //TODO here need to set shader on effectsrenderer
}

void PostEvent::update()
{
    //TODO here, need to set fade attribute on effectsrenderer and then revert to previous post shader after it dies
}

void PostEvent::draw()
{    
    // nothing
}