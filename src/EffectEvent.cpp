//
//  EffectEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "EffectEvent.h"
#include <boost/foreach.hpp>

EffectEvent::~EffectEvent()
{    
    mAttributes.clear();
}

void EffectEvent::start()
{
    mIsStarted = true; 
    mIsStopping = false;
    mIsStopped = false;
    mStartTime = getElapsedSeconds();
}

void EffectEvent::stop(bool hardStop)
{
    mIsStopping = true;
    mHardStop = hardStop;
}

float EffectEvent::getEventElapsedSeconds()
{
    if (mStartTime == -1.0f)
        return 0.0f;
        
    return getElapsedSeconds () - mStartTime;
}

void EffectEvent::registerAttribute(string attrName, string attrType)
{
    EffectAttribute newAttr;
    newAttr.mName = attrName;
    newAttr.mType = attrType;
    
    mAttributes[attrName] = newAttr;
}


//void EffectEvent::stop()