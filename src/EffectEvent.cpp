//
//  EffectEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "EffectEvent.h"

namespace bloom { namespace effects {

using namespace std;
using namespace ci;

EffectEvent::~EffectEvent()
{    
    mAttributes.clear();
}

void EffectEvent::start()
{
    mEventState = EVENT_STARTED;
    mActualStartTime = mTimer.getSeconds();
    mActualSeconds = 0.0f;
    mPreviousElapsed = 0.0f;
}

void EffectEvent::stop(bool hardStop)
{
    if (mEventState != EVENT_STOPPED)
        mEventState = EVENT_STOPPING;
        
    mHardStop = hardStop;
    
    if (hardStop)
    {
        mEventState = EVENT_STOPPED;
        mActualStartTime = -1.0f;
        mActualSeconds = 0.0f;
    }
}

double EffectEvent::getEventElapsedSeconds()
{
    if (mActualStartTime == -1.0f)
    {
        mActualSeconds = 0.0f;
        return mActualSeconds;
    }
        
    double elapsed = mTimer.getSeconds() - mActualStartTime;
    double deltaTime = elapsed - mPreviousElapsed;
    
    if (deltaTime > ELAPSED_MAX)
    {
        deltaTime = ELAPSED_MAX;
    }
    
    mPreviousElapsed = elapsed;
    mActualSeconds += deltaTime;
    
    return mActualSeconds;
}

void EffectEvent::registerAttribute(string attrName, AttributeType attrType)
{
    EffectAttribute newAttr;
    newAttr.mName = attrName;
    newAttr.mType = attrType;
    
    mAttributes[attrName] = newAttr;
}

EffectAttribute EffectEvent::getAttribute(string attrName)
{
    if (mAttributes.find(attrName) != mAttributes.end())
    {
      return mAttributes.at(attrName);
    }
    
    return EffectAttribute();
}

void EffectEvent::updateSource()
{
    if (mEventState == EVENT_STOPPED || mEventState == EVENT_INITIALIZED)
    {
        mActualStartTime = -1.0f;
    }
        
    if (mParentTransformChanged)
    {
        mSourcePosition = mParentTransform.transformPoint(mLocalPosition);
        mSourceOrientation = mLocalOrientation * Quatf(mParentTransform);
        mParentTransformChanged = false;
    }
}

}} // namespace bloom::effects