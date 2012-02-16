//
//  PostEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "PostEvent.h"

// may want texture input later
//#include "cinder/gl/Texture.h"
//#include "cinder/gl/gl.h"

namespace bloom { namespace effects {

using namespace ci;
using namespace std;

const std::string PATH_EXTENSION = ".post.json";

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
void PostEvent::setup(ci::Vec2f windowSize)
{ 
    processAttributes();
}

void PostEvent::update()
{
    updateSource();
    
    float totalElapsed = getEventElapsedSeconds();
    //float dt = totalElapsed - mPreviousElapsed;  
    
    // run through the state gauntlet
    if (isStopped())
    {
        mRenderManager->setDefaultPostShader();
        mRenderManager->setPostShaderAlpha(1.0f);
        
        mEventState = EVENT_INITIALIZED;
        return;
    }
    else if (isInitialized() || totalElapsed == 0.0f)
    {
        return;
    }    
    else if (isStopping())
    {
        if (mFadeTime.y > 0.0f)
        {
            if (mFadeStartTime == -1.0f)
            {
                mFadeStartTime = totalElapsed;
            }


            float fadeAmt = 1.0f - math<float>::clamp((totalElapsed - mFadeStartTime)/mFadeTime.y, 0.0f, 1.0f);
            mRenderManager->setPostShaderAlpha(fadeAmt);
            
            if (fadeAmt <= 0.0f)
            {
                mFadeStartTime = -1.0f;
                mEventState = EVENT_STOPPED;
            }
        }
        else
        {
            mEventState = EVENT_STOPPED;
        }
    }
    else if (isStarted())
    {
        mEventState = EVENT_RUNNING;

        //TODO here need to set shader on effectsrenderer
        if (mRenderManager)
            mRenderManager->setPostShader(mShaderName);
    }
    
    //-------------------------------
    
    if (isRunning())
    {
        float tailFadeRange = mLifetime - mFadeTime.y;
        
        if (totalElapsed >= mLifetime && mLifetime != -1.0f)
        {
            stop(true);
            return;
        }
        // set the post alpha parameter, which is optionally used by the shader
        else if (totalElapsed < mFadeTime.x)
        {
            float fadeAmt = totalElapsed / mFadeTime.x;
            mRenderManager->setPostShaderAlpha(fadeAmt);
        }
        else if (totalElapsed > tailFadeRange && mLifetime != -1.0f)
        {
            float fadeAmt = 1.0f - math<float>::clamp((totalElapsed - tailFadeRange)/mFadeTime.y, 0.0f, 1.0f);
            mRenderManager->setPostShaderAlpha(fadeAmt);
        }
    }

    
    //mPreviousElapsed = totalElapsed;
}

void PostEvent::draw(bool enabled)
{    
    // nothing
}

}} // namespace bloom::effects