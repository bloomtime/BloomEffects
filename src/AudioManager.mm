//
//  AudioManager.mm
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//


#include "AudioManager.h"

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
}


void AudioManager::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

FMOD::Sound* AudioManager::createSound(string filepath)
{
    FMOD::Sound *newSound;
    
    FMOD_RESULT result        = FMOD_OK;
    char        buffer[200]   = {0};
    
    [[NSString stringWithFormat:@"%@/swish.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = mSystem->createSound(buffer, FMOD_SOFTWARE, NULL, &newSound);
    ERRCHECK(result);
    
    return newSound;
}

void AudioManager::playSound(FMOD::Sound* sound) 
{
    FMOD_RESULT result = FMOD_OK;

    result = mSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &mChannel1);
    ERRCHECK(result);
}

bool AudioManager::isPlaying(FMOD::Sound* sound)
{
    FMOD_RESULT result = FMOD_OK;
    bool isPlaying = false;
    FMOD::Sound *currentSound;

    result = mChannel1->isPlaying(&isPlaying);
    ERRCHECK(result);
    
    if (isPlaying)
    {
        result = mChannel1->getCurrentSound(&currentSound);
        ERRCHECK(result);
        
        if (currentSound == sound)
            return true;
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
}
