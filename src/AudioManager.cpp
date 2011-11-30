//
//  AudioManager.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "cinder/app/App.h"
#include "AudioManager.h"
#include <boost/filesystem.hpp>

AudioManager::AudioManager()
{
    mSystem = NULL;
    mIs3D = false;
    
    memset(mChannels, 0, sizeof(mChannels));
}

AudioManager::~AudioManager()
{
    mSystem->release();

    for (int i=0; i < 6; i++)
    {
        mChannels[i] = 0;
    }
}


void AudioManager::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

FMOD::Sound* AudioManager::createSound(string filepath, bool looping)
{
    FMOD::Sound *newSound;
    FMOD_RESULT result = FMOD_OK;
    
    string fullPath = ci::app::App::getResourcePath(filepath).string();
    
    int options = FMOD_SOFTWARE;
    
    if (mIs3D)
        options |= FMOD_3D;
    
    if (looping)
        options |= FMOD_LOOP_NORMAL;
    
    result = mSystem->createSound(fullPath.c_str(), options , NULL, &newSound);
    ERRCHECK(result);
    
    return newSound;
}

void AudioManager::playSound(FMOD::Sound* sound, float volume, Vec3f pos, Vec3f vel) 
{
    FMOD_RESULT result = FMOD_OK;
    
    FMOD_VECTOR currentPos = { pos[0], pos[1], pos[2] };
    FMOD_VECTOR currentVel = { vel[0], vel[1] ,vel[2] };
        
    for (int i=0; i < MAX_CHANNELS; i++)
    {   
        bool isPlaying = false;
        
        if (mChannels[i] != NULL)
        {
            result = mChannels[i]->isPlaying(&isPlaying);
            ERRCHECK(result);
        }
        
        if (!isPlaying)
        {
            mChannels[i] = NULL;
            
            result = mSystem->playSound(FMOD_CHANNEL_FREE, sound, true, &mChannels[i]);
            ERRCHECK(result);
            
            if (mIs3D)
            {
                result = mChannels[i]->set3DAttributes(&currentPos, &currentVel);
                ERRCHECK(result);
            }
            
            mChannels[i]->setVolume(volume);
            
            result = mChannels[i]->setPaused(false);
            ERRCHECK(result);
            return;
        }
    }
    
    // for now, if all channels are full, then don't play the sound
}

void AudioManager::stopSound(FMOD::Sound* sound)
{
    FMOD_RESULT result = FMOD_OK;
    FMOD::Sound *currentSound;
        
    for (int i=0; i < MAX_CHANNELS; i++)
    {
        if (mChannels[i] != NULL)
        {
            result = mChannels[i]->getCurrentSound(&currentSound);
            ERRCHECK(result);
    
            bool isPlaying;
            result = mChannels[i]->isPlaying(&isPlaying);
            ERRCHECK(result);
            
            if (currentSound == sound && isPlaying)
            {
                mChannels[i]->stop();
                mChannels[i] = NULL;
                return;
            }
        }
    }
}

void AudioManager::setVolume(FMOD::Sound* sound, float volume)
{
    FMOD_RESULT result = FMOD_OK;
    FMOD::Sound *currentSound;
        
    for (int i=0; i < MAX_CHANNELS; i++)
    {
        if (mChannels[i] != NULL)
        {
            result = mChannels[i]->getCurrentSound(&currentSound);
            ERRCHECK(result);
            
            bool isPlaying;
            result = mChannels[i]->isPlaying(&isPlaying);
            ERRCHECK(result);
    
            if (currentSound == sound && isPlaying)
                mChannels[i]->setVolume(volume);
        }
    }
}

bool AudioManager::isPlaying(FMOD::Sound* sound)
{
    FMOD_RESULT result = FMOD_OK;
    FMOD::Sound *currentSound;
        
    for (int i=0; i < MAX_CHANNELS; i++)
    {
        if (mChannels[i] != NULL)
        {
            result = mChannels[i]->getCurrentSound(&currentSound);
            ERRCHECK(result);
    
            bool isPlaying;
            result = mChannels[i]->isPlaying(&isPlaying);
            ERRCHECK(result);
            
            if (currentSound == sound && isPlaying)
                return true;
        }
    }

    return false;
}

void AudioManager::setup()
{
    FMOD_RESULT   result        = FMOD_OK;
    //char          buffer[200]   = {0};
    unsigned int  version       = 0;

    /*
        Create a mSystem object and initialize
    */    
    result = FMOD::System_Create(&mSystem); 
    ERRCHECK(result);
    
    result = mSystem->getVersion(&version);
    ERRCHECK(result);
    
    if (version < FMOD_VERSION)
    {
        fprintf(stderr, "You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }
    
    result = mSystem->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL);
    ERRCHECK(result);
    
    updateListenerPosition();
}

void AudioManager::updateListenerPosition()
{
    if (mIs3D)
    {
        FMOD_RESULT   result        = FMOD_OK; 
        Vec3f trans = mCamera->getEyePoint();
        FMOD_VECTOR listenerpos = { trans[0], trans[1], trans[2] };
        
        result = mSystem->set3DListenerAttributes(0, &listenerpos,NULL,NULL,NULL);
        ERRCHECK(result);
    }
}

void AudioManager::update()
{
    updateListenerPosition();
    
    FMOD_RESULT result = FMOD_OK;
        
    for (int i=0; i < MAX_CHANNELS; i++)
    {
        if (mChannels[i] != NULL)
        {
            bool isPlaying;
            result = mChannels[i]->isPlaying(&isPlaying);
            ERRCHECK(result);
            
            if (!isPlaying)
                mChannels[i] = NULL;
        }
    }
    
    /*
    FMOD_RESULT result          = FMOD_OK;
    //int         channelsplaying = 0;
    
    if (system != NULL)
    {
        //result = mSystem->getChannelsPlaying(&channelsplaying);
        //ERRCHECK(result);
        
        result = mSystem->update();
        ERRCHECK(result);
    }
    */
}
