//
//  PostEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "PostEvent.h"

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
    mShaderName("defaultPost"),
    mFadeStartTime(-1.0f)
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
    mShaderName = mAttributes.at("Shader").getString();
}
void PostEvent::setup()
{ 
    processAttributes();
}

void PostEvent::update()
{
    float totalElapsed = getEventElapsedSeconds();
    //float dt = totalElapsed - mPreviousElapsed;  
    
    // run through the state gauntlet
    if (isInitialized() || isStopped())
    {
        return;
    }    
    else if (isStarted())
    {
        mEventState = EVENT_RUNNING;

        //TODO here need to set shader on effectsrenderer
        if (mState)
            mState->setPostShader(mShaderName);
    }
    // -1.0f lifetime is infinite lifetime
    else if (isRunning() && totalElapsed >= mLifetime && mLifetime != -1.0f)
    {
        stop(true);
        
        mState->setDefaultPostShader();
        mState->setPostShaderAlpha(1.0f);
        return;
    }
    
    float tailFadeRange = mLifetime - mFadeTime.y;
    
    // set the post alpha parameter, which is optionally used by the shader
    if (totalElapsed < mFadeTime.x)
    {
        float fadeAmt = totalElapsed / mFadeTime.x;
        mState->setPostShaderAlpha(fadeAmt);
    }
    else if (isRunning() && mLifetime != -1.0f && totalElapsed > tailFadeRange)
    {
        float fadeAmt = 1.0f - math<float>::clamp((totalElapsed - tailFadeRange)/mFadeTime.y, 0.0f, 1.0f);
        mState->setPostShaderAlpha(fadeAmt);
    }
    else if (isStopping() && mFadeTime.y > 0.0f)
    {
        if (mFadeStartTime == -1.0f)
        {
            mFadeStartTime = totalElapsed;
        }
        else
        {
            float fadeAmt = 1.0f - math<float>::clamp((totalElapsed - mFadeStartTime)/mFadeTime.y, 0.0f, 1.0f);
            mState->setPostShaderAlpha(fadeAmt);
            
            if (fadeAmt == 0.0f)
                mEventState = EVENT_STOPPED;
        }
    }
    
    //mPreviousElapsed = totalElapsed;
}

void PostEvent::draw()
{    
    // nothing
}