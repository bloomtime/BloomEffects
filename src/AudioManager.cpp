//
//  AudioManager.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "cinder/app/App.h"
#include "AudioManager.h"
#include <boost/filesystem.hpp>

namespace bloom { namespace effects {

using namespace std;
using namespace ci;

AudioManager::AudioManager()
{
    mSystem = NULL;
    mEventSystem = NULL;
    mIs3D = false;
    
    memset(mChannels, 0, sizeof(mChannels));
}

AudioManager::~AudioManager()
{
    if (mEventSystem)
    {
        mEventSystem->release();
        mEventSystem = NULL;
        mSystem = NULL;
    }
    
    for (int i=0; i < 6; i++)
    {
        mChannels[i] = 0;
    }
}


void AudioManager::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        //disabled for now--too vomitous
        //fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        //exit(-1);
    }
}

void AudioManager::loadFEV(string filepath)
{
    FMOD_RESULT result = FMOD_OK;
    string fullPath = app::App::getResourcePath(filepath).string();
    result = mEventSystem->load(fullPath.c_str(), NULL, NULL);
    ERRCHECK(result);
}

FMOD::Event* AudioManager::createEvent(string eventPath)
{
    FMOD::Event *event;
    
    FMOD_RESULT result = FMOD_OK;
    result = mEventSystem->getEvent(eventPath.c_str(), FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    return event;
}

void AudioManager::playEvent(FMOD::Event* event, float volume, Vec3f pos, Vec3f vel) 
{
    if (event == NULL)
        return;
        
    FMOD_VECTOR currentPos = { pos[0], pos[1], pos[2] };
    FMOD_VECTOR currentVel = { vel[0], vel[1] ,vel[2] };
    FMOD_RESULT result = FMOD_OK;
    
    event->setVolume(volume);
    
    if (mIs3D)
        event->set3DAttributes(&currentPos, &currentVel);
        
    result = event->start();
    ERRCHECK(result);
}

bool AudioManager::isEventPlaying(FMOD::Event* event)
{
    if (event == NULL)
        return false;
        
    FMOD_EVENT_STATE state = {0};
    FMOD_RESULT result = FMOD_OK;
    result = event->getState(&state);
    ERRCHECK(result);
    
    return state & FMOD_EVENT_STATE_PLAYING;
}

FMOD::Sound* AudioManager::createSound(string filepath, bool looping)
{
    if (mSystem == NULL)
        return NULL;
        
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
    if (mSystem == NULL || sound == NULL)
        return;
        
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
    if (mSystem == NULL || sound == NULL)
        return;
        
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

void AudioManager::setSoundVolume(FMOD::Sound* sound, float volume)
{
    if (mSystem == NULL || sound == NULL)
        return;
        
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

bool AudioManager::isSoundPlaying(FMOD::Sound* sound)
{
    if (mSystem == NULL || sound == NULL)
        return false;
        
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

    /*
        Create an FMOD Event System object and initialize
    */
    result = FMOD::EventSystem_Create(&mEventSystem); 
    ERRCHECK(result);
    
    result = mEventSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    
    result = mEventSystem->getSystemObject(&mSystem);
    ERRCHECK(result);
    
    updateListenerPosition();

}

void AudioManager::updateListenerPosition()
{
    if (mIs3D && mSystem != NULL)
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
        if (mChannels[i])
        {
            bool isPlaying;
            result = mChannels[i]->isPlaying(&isPlaying);
            ERRCHECK(result);
            
            if (!isPlaying)
                mChannels[i] = NULL;
        }
    }
    
    if (mEventSystem)
    {
        result = mEventSystem->update();
        ERRCHECK(result);
    }
    
    //int         channelsplaying = 0;
    
    if (mSystem)
    {
        //result = mSystem->getChannelsPlaying(&channelsplaying);
        //ERRCHECK(result);
        
        result = mSystem->update();
        ERRCHECK(result);
    }
}

}} // namespace bloom::effects
