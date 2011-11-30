//
//  SoundEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "SoundEvent.h"

// may want texture input later
//#include "cinder/gl/Texture.h"
//#include "cinder/gl/gl.h"

using namespace ci;

const string PATH_EXTENSION = ".sound.json";

SoundEventRef SoundEvent::create(AudioManagerRef audioMgr)
{
    return SoundEventRef( new SoundEvent(audioMgr) );
}

SoundEvent::SoundEvent(AudioManagerRef audioMgr):
    mFilePath(""),
    mFadeTime(0.0f),
    mPlayMode(PLAY_ONCE),
    mVolume(1.0f)
{ 
    mLifetime = 0.0f;
    
    mAudioManager = audioMgr;
    mFileExtension = PATH_EXTENSION;
    registerAttributes(); 
}
    
SoundEvent::~SoundEvent()
{
    mSound->release();
}

void SoundEvent::processAttributes()
{
    mFilePath = mAttributes.at("Path").getString();
    mLifetime = mAttributes.at("Lifetime").getFloat();
    mFadeTime = mAttributes.at("FadeTime").getFloat();
    mVolume = mAttributes.at("Volume").getFloat();
    mPlayMode = PLAY_MODES.at(mAttributes.at("PlayMode").getString());
}

void SoundEvent::setup()
{ 
    processAttributes();
        
    mSound = mAudioManager->createSound(mFilePath, mPlayMode == PLAY_LOOPING);
}

void SoundEvent::doFade(float elapsed, float lifetime)
{
    if (mFadeTime == 0.0f)
    {
        return;
    }

    float fadeStartTime = math<float>::clamp(lifetime - mFadeTime, 0.0f, lifetime);
    float fadeAmount = math<float>::clamp(elapsed - fadeStartTime, 0.0f, lifetime);
    
    float totalFade = mVolume * (1.0f - (fadeAmount / mFadeTime));
    
    //fade out based on lifetime and mFadetime
    mAudioManager->setVolume(mSound, totalFade);
}

void SoundEvent::update()
{
    updateSource();
    
    float totalElapsed = getEventElapsedSeconds();
    
    // run through the state gauntlet
    if (isInitialized() || isStopped())
    {
        if (mAudioManager->isPlaying(mSound))
        {
           mAudioManager->stopSound(mSound);
        }  
        
        return;
    }  
    else if (isStopping())
    {
        if (!mAudioManager->isPlaying(mSound))
        {
            mEventState = EVENT_STOPPED;
        }  
        
        //handle the looping infinite case 
        //set the lifetime and then fade it out
        if (mPlayMode == PLAY_LOOPING)
        {
            mLifetime = totalElapsed + mFadeTime;
            mEventState = EVENT_RUNNING;
        }
    }
    else if (isRunning())
    { 
        if (mPlayMode == PLAY_ONCE)
        {
            if (mLifetime == 0.0f)
            {
                unsigned int lengthMS;
                mSound->getLength(&lengthMS, FMOD_TIMEUNIT_MS);
            
                mLifetime = lengthMS * .001f; 
            }
            
            if (totalElapsed > mLifetime)
            {
                mAudioManager->stopSound(mSound);
                mEventState = EVENT_STOPPING;
                return;
            }
            
            doFade(totalElapsed, mLifetime);
        
        }
        else if (mPlayMode == PLAY_LOOPING)
        {
            if (mLifetime != 0.0f)
            {
                if (totalElapsed > mLifetime)
                {
                    mAudioManager->stopSound(mSound);
                    mEventState = EVENT_STOPPING;
                    return;
                }
                
                doFade(totalElapsed, mLifetime);
            }
        }
    }
    else if (isStarted())
    {
        //play the sound
        mAudioManager->playSound(mSound, mVolume, mSourcePosition);
        
        mEventState = EVENT_RUNNING;
    }
}

void SoundEvent::draw()
{    
    // nothing
}