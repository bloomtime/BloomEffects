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

void Effect::initializeData(Json::Value data)
{
    if (data != NULL) {
        vector<Json::Value> events = effects::readVector(data, "Events");
        
        vector<Json::Value>::iterator it;
        
        for ( it = events.begin(); it != events.end(); it++)
        {
            Json::Value currentBlock = *it;
            string eventType = currentBlock["EventType"].asString();
            string eventPath = currentBlock["EventPath"].asString();
            
            bool enabled = currentBlock["Enabled"].asBool();
            
            if (!enabled)
                continue;
            
            if (eventType == "" || eventPath == "")
                break;
            
            //console() << "EVENT: " << EventPath << " - " << EventType << std::endl;
            
            ChildEvent ChildType = CHILD_EVENTS.at(eventType.c_str());
            EffectEventRef currentEvent;
            
            // handle the different types here
            switch (ChildType) 
            {
                case PARTICLE_EVENT:
                {
                    currentEvent = ParticleEvent::create();
                    // need to concatenate extension here
                    break;
                }
                default:
                {
                    //nothing!
                    console() << "WARNING: Unrecognized EventType" << eventType << std::endl;
                    return;                
                }
            }
            
            // Parse child event path
            if (currentEvent)
            {
                // Parse global parameters
                currentEvent->setEnabled(enabled);
                
                currentEvent->setStartTime(currentBlock["StartTime"].asFloat());
            
                vector<Json::Value> posValues = effects::readVector(currentBlock, "Position");
                currentEvent->setSourcePosition(Vec3f(posValues[0].asFloat(), posValues[1].asFloat(), posValues[2].asFloat()));       
            
                vector<Json::Value> orientValues = effects::readVector(currentBlock, "Orientation");
                currentEvent->setSourceOrientation(Vec3f(orientValues[0].asFloat(), orientValues[1].asFloat(), orientValues[2].asFloat()));
                
                eventPath.append(currentEvent->getPathExtension());
                Json::Value childData = effects::getJsonData(eventPath);
                
                //console() << "EventPath "<< EventPath << std::endl;
                
                BOOST_FOREACH(EffectAttrMap::value_type i, currentEvent->getAttributes()) 
                {
                    parseAttr(childData, i.second, currentEvent);
                }            
    
                //addChild( EffectEventRef(currentEvent) );
                mEvents.push_back(currentEvent);
            }
        }
    }
    else {
        console() << "ERROR: Invalid Data."<< std::endl;
    }
}

void Effect::parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent)
{
    //console() << "Attr Type is: " << attr.type << std::endl;
    
    boost::any currentValue;
    
    switch (attr.mType) 
    {
        case ATTR_COLOR:
        {
            vector<Json::Value> values = effects::readVector(data, attr.mName);
            currentValue = Color(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
            break;
        }
        case ATTR_TEXTURE:
        {
            currentValue = data[attr.mName].asString();
            break;
        }
        case ATTR_SHADER:
        {
            currentValue = data[attr.mName].asString();
            break;
        }
        case ATTR_FLOAT:
        {
            currentValue = data[attr.mName].asFloat();
            break;
        }
        case ATTR_VECTOR3:
        {
            vector<Json::Value> values = effects::readVector(data, attr.mName);
            currentValue = Vec3f(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
            break;
        }
        case ATTR_VECTOR2:
        {
            vector<Json::Value> values = effects::readVector(data, attr.mName);
            currentValue = Vec2f(values[0].asFloat(), values[1].asFloat());
            break;
        }
        case ATTR_BOOL:
        {
            currentValue = data[attr.mName].asBool();
            break;
        }
        case ATTR_STRING:
        {
            currentValue = data[attr.mName].asString();
            break;
        }
        case ATTR_CURVE:
        {
            floatCurvePoints currentCurve;
        
            vector<Json::Value> pointValues = effects::readVector(data, attr.mName);
        
            for (vector<Json::Value>::const_iterator it = pointValues.begin(); it != pointValues.end(); ++it)
            {
                vector<Json::Value> pValues = effects::readVector((*it), "point");
                Vec3f currentPoint = Vec3f(pValues[0].asFloat(), pValues[1].asFloat(), pValues[2].asFloat());
                currentCurve.push_back(currentPoint);
            
                //console() << "p is " << currentPoint[0] << " " << currentPoint[1] << std::endl;
            }
        
            currentValue = currentCurve;
            break;
        }
        default:
        {
            console() << "ERROR:  Unrecognized Attr Type" << std::endl;
            break;               
        }
    }

    currentEvent->setAttribute(attr.mName, currentValue);
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

void Effect::setup(Json::Value data)
{
    initializeData(data);
    
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