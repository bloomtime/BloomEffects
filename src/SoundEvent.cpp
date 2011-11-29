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
    mFilePath("")
{ 
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
}

void SoundEvent::setup()
{ 
    processAttributes();
    
    mSound = mAudioManager->createSound(mFilePath);
}

void SoundEvent::update()
{
    //float totalElapsed = getEventElapsedSeconds();
    //float dt = totalElapsed - mPreviousElapsed;  
    
    // run through the state gauntlet
    if (isInitialized() || isStopped())
    {
        return;
    }  
    else if (isStopping())
    {
        if (!mAudioManager->isPlaying(mSound))
            mEventState = EVENT_STOPPED;
    }
    else if (isStarted())
    {
        mEventState = EVENT_RUNNING;
        
        //play the sound
        mAudioManager->playSound(mSound);
        
        stop(false);
    }
    
    /* TODO not needed yet--only playing a single sound
    // -1.0f lifetime is infinite lifetime
    else if (isRunning() && totalElapsed >= mLifetime && mLifetime != -1.0f)
    {
        stop(true);

        return;
    }
    */
    
    //mPreviousElapsed = totalElapsed;
}

void SoundEvent::draw()
{    
    // nothing
}