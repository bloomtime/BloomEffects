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
	for( list<EffectEventRef>::iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        it = mEvents.erase( it );
    }
}

void Effect::start()
{
    if (isRunning() || isStarted())
    {
        return;
    }
    
    mIsVisible = true;
    mEffectState = EFFECT_STARTED;
    mStartedTime = mTimer.getSeconds();
    mIsChildrenRunning = false;
    mActualSeconds = 0.0f;
    mPreviousElapsed = mStartedTime;
}

void Effect::stop(bool hardStop)
{
    for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
            (*it)->stop(hardStop);
    }
}

double Effect::getEffectElapsedSeconds()
{
    if (mStartedTime == -1.0f)
        return 0.0f;
        
    double elapsed = mTimer.getSeconds();
    double deltaTime = elapsed - mPreviousElapsed;
    
    if (deltaTime > ELAPSED_MAX)
    {
        deltaTime = ELAPSED_MAX;
    }
    
    mPreviousElapsed = elapsed;
    mActualSeconds += deltaTime;
    
    return mActualSeconds;
}

void Effect::setup(list<EffectEventRef> events, Vec2f windowSize)
{   
    mEvents = events;
    
	for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
        {
            (*it)->setParentTransform(mTransform);
            (*it)->setCamera(mCamera);
            (*it)->setup(windowSize);
        }
    }
}

void Effect::update()
{
    if (isStarted())
    {
        for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
        {
            (*it)->setInitialized();
        }
    
        mEffectState = EFFECT_RUNNING;
    }
    
    if (mIsVisible && isRunning()) {         
        
        mIsChildrenRunning = false;
        
        // update children
        for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
        {
            if ((*it)->isInitialized() && (getEffectElapsedSeconds() >= (*it)->getToStartTime()))
            {
                (*it)->start();
            }
            
            
            if ((*it)->isEnabled())
                (*it)->update();
                
            if (!(*it)->isStopped())
            {
                mIsChildrenRunning = true;
            }
        }
        
        // if no children left, stop self
        if (!mIsChildrenRunning)
        {
            mEffectState = EFFECT_STOPPED;
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

void Effect::setTexture(ci::gl::Texture texture, int ID)
{
	for( list<EffectEventRef>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
        {
            (*it)->setTexture(texture, ID);
        }
    }
}

void Effect::setCamera(CameraRef camera)
{
    mCamera = camera;
}