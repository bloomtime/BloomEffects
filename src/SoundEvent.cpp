//
//  SoundEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "SoundEvent.h"

namespace bloom { namespace effects {

using namespace ci;
using namespace std;

const string PATH_EXTENSION = ".sound.json";

SoundEventRef SoundEvent::create(AudioManagerRef audioMgr)
{
    return SoundEventRef( new SoundEvent(audioMgr) );
}

SoundEvent::SoundEvent(AudioManagerRef audioMgr):
    mFilePath(""),
    mFadeTime(0.0f),
    mPlayMode(PLAY_ONCE),
    mVolume(1.0f),
    mEvent(NULL),
    mSound(NULL),
    mIsInfinite(false)
{ 
    mLifetime = 0.0f;
    
    mAudioManager = audioMgr;
    mFileExtension = PATH_EXTENSION;
    registerAttributes(); 
}
    
SoundEvent::~SoundEvent()
{
    if (mEvent)
    {
        mEvent->release();
        mEvent = NULL;
    }
    
    if (mSound)
    {
        mSound->release();
        mSound = NULL;
    }
}

void SoundEvent::processAttributes()
{
    mFilePath = mAttributes.at("Path").getString();
    mLifetime = mAttributes.at("Lifetime").getFloat();
    mFadeTime = mAttributes.at("FadeTime").getFloat();
    mVolume = mAttributes.at("Volume").getFloat();
    mPlayMode = PLAY_MODES.at(mAttributes.at("PlayMode").getString());
    mType = SOUND_TYPES.at(mAttributes.at("Type").getString());
    
    mIsInfinite = false;
    
    if (mLifetime == 0.0f)
        mIsInfinite = true;
}

void SoundEvent::setup(ci::Vec2f windowSize)
{ 
    processAttributes();
        
    if (mType == SOUNDTYPE_FILE)
        mSound = mAudioManager->createSound(mFilePath);
    else
        mEvent = mAudioManager->createEvent(mFilePath);
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
    if (mType == SOUNDTYPE_FILE)
        mAudioManager->setSoundVolume(mSound, totalFade);
    else
        mEvent->setVolume(totalFade);
}

void SoundEvent::update()
{
    updateSource();
    
    float totalElapsed = getEventElapsedSeconds();
    
    // run through the state gauntlet
    if (isInitialized() || isStopped())
    {
        if (mAudioManager->isSoundPlaying(mSound))
        {
           mAudioManager->stopSound(mSound);
        }  
        
        if (mAudioManager->isEventPlaying(mEvent))
        {
           mEvent->stop(true);
        } 
        
        if (mPlayMode == PLAY_LOOPING)
            mLifetime = 0.0f;
        
        return;
    }  
    else if (isStopping())
    {
        if (!mAudioManager->isSoundPlaying(mSound) && !mAudioManager->isEventPlaying(mEvent))
        {
            mEventState = EVENT_STOPPED;
            
            if (mPlayMode == PLAY_LOOPING)
                mLifetime = 0.0f;
            return;
        }  
        
        //handle the looping infinite case 
        //set the lifetime and then fade it out
        if (mPlayMode == PLAY_LOOPING && mIsInfinite)
        {
            if (mLifetime == 0.0f)
                mLifetime = totalElapsed + mFadeTime;
            else if (totalElapsed > mLifetime)
            {
                mAudioManager->stopSound(mSound);
                
                if (mEvent)
                    mEvent->stop();
                    
                return;
            }
                
            doFade(totalElapsed, mLifetime);
            //mLifetime = totalElapsed + mFadeTime;
            //mEventState = EVENT_RUNNING;
        }
    }
    else if (isRunning())
    { 
        if (mPlayMode == PLAY_ONCE)
        {
            if (mLifetime == 0.0f)
            {
                unsigned int lengthMs;
                
                if (mType == SOUNDTYPE_FILE)
                {
                    mSound->getLength(&lengthMs, FMOD_TIMEUNIT_MS);
                }
                else
                {
                    FMOD_EVENT_INFO info;
                    mEvent->getInfo(0,0, &info);
                    lengthMs = info.lengthms;
                }
                
                mLifetime = lengthMs * .001f; 
            }
            
            if (totalElapsed > mLifetime)
            {
                mAudioManager->stopSound(mSound);
                
                if (mEvent)
                    mEvent->stop();
                    
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
                    if (mType == SOUNDTYPE_FILE)
                        mAudioManager->stopSound(mSound);
                    else if (mEvent)
                        mEvent->stop();
                        
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
        if (mType == SOUNDTYPE_FILE)
            mAudioManager->playSound(mSound, mVolume, mSourcePosition);
        else
            mAudioManager->playEvent(mEvent, mVolume, mSourcePosition);
        
        mEventState = EVENT_RUNNING;
    }
}

void SoundEvent::draw(bool enabled)
{    
    // nothing
}

}} // namespace bloom::effects