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
    mCamera = NULL;
}

void EffectEvent::start()
{
    mEventState = EVENT_STARTED;
    mStartTime = getElapsedSeconds();
}

void EffectEvent::stop(bool hardStop)
{
    mEventState = EVENT_STOPPING;
    mHardStop = hardStop;
    
    if (hardStop)
        mEventState = EVENT_STOPPED;
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

void EffectEvent::updateEmitter()
{
    if (mParentTransformChanged)
    {
        mEmitterPosition = mParentTransform.transformPoint(mLocalPosition);
        mEmitterOrientation = mLocalOrientation * Quatf(mParentTransform);
        mParentTransformChanged = false;
    }
}
