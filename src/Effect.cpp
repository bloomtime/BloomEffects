//
//  Effect.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "Effect.h"

#include <boost/foreach.hpp>
#include <boost/any.hpp>

EffectRef Effect::create()
{
    return EffectRef( new Effect() );
}

void Effect::setTransform( const ci::Matrix44f &transform ) 
{ 
    mTransform = transform; 
    
	for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
        {
            (*it)->setParentTransform(mTransform);
        }
    }
}


Effect::~Effect()
{
    mCamera = NULL;
    
	for( list<EffectEventRef>::iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        it = mEvents.erase( it );
    }
}

void Effect::start()
{
    mIsStarted = true;
    mStartedTime = getElapsedSeconds();
}

void Effect::stop(bool hardStop)
{
    for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
            (*it)->stop(hardStop);
    }
}

float Effect::getEffectElapsedSeconds()
{
    if (mStartedTime == -1.0f)
        return 0.0f;
        
    return getElapsedSeconds () - mStartedTime;
}

void Effect::setup(list<EffectEventRef> events)
{   
    mEvents = events;
    
	for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
        {
            (*it)->setParentTransform(mTransform);
            (*it)->setCamera(mCamera);
            (*it)->setup();
        }
    }
}

void Effect::update()
{
    if (mIsVisible && mIsStarted) {
        // clean up any children
        for( list<EffectEventRef>::iterator it = mEvents.begin(); it != mEvents.end(); ++it )
        {
            if ((*it)->isStopped())
            {
                it = mEvents.erase( it );
            }
        }          
    
        // if no children left, stop self
        if (mIsStarted && mEvents.size() == 0)
            mIsStopped = true;
        
        // update children
        for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
        {
            if ((*it)->isInitialized() && (getEffectElapsedSeconds() >= (*it)->getStartTime()))
            {
                (*it)->start();
            }
            
            
            if ((*it)->isEnabled())
                (*it)->update();
        }
        
    }
}

void Effect::draw()
{    
    for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
            (*it)->draw();
    }
}

void Effect::setTintColor(Vec3f color)
{
	for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
        {
            (*it)->setTintColor(color);
        }
    }
}

void Effect::setSourceScale(float scale)
{
	for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
        {
            (*it)->setSourceScale(scale);
        }
    }
}

void Effect::setCamera(ci::CameraPersp *camera)
{
    mCamera = camera;
}