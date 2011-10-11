//
//  Effect.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "Effect.h"
#include "ParticleEvent.h"

#include <boost/foreach.hpp>
#include <boost/any.hpp>

Effect::Effect(string effectPath)
{
    mIsVisible = true;
    mData = getData(effectPath);
}

json::Value Effect::getData(string effectPath)
{
    DataSourceRef dataSource = loadResource(effectPath);
    Buffer buf = dataSource->getBuffer();
	size_t dataSize = buf.getDataSize();
	shared_ptr<char> bufString( new char[dataSize+1], checked_array_deleter<char>() );
	memcpy( bufString.get(), buf.getData(), buf.getDataSize() );
	bufString.get()[dataSize] = 0;
    
    return json::deserialize(bufString.get());
}

void Effect::initializeData()
{
    if (mData != NULL) {
        vector<json::Value> events = json::readValues(mData, "Events");
        
        vector<json::Value>::iterator it;
        
        for ( it = events.begin(); it != events.end(); it++)
        {
            json::Value currentBlock = *it;
            string EventType = currentBlock["EventType"].asString();
            string EventPath = currentBlock["EventPath"].asString();
            
            if (EventType == "" || EventPath == "")
                break;
            
            //console() << "EVENT: " << EventPath << " - " << EventType << std::endl;
            
            ChildEvent ChildType = CHILD_EVENTS.at(EventType.c_str());
            EffectEvent* currentEvent = NULL;
            
            // handle the different types here
            switch (ChildType) 
            {
                case PARTICLE_EVENT:
                {
                    currentEvent = new ParticleEvent();
                    // need to concatenate extension here
                    break;
                }
                default:
                {
                    //nothing!
                    console() << "WARNING: Unrecognized EventType" << EventType << std::endl;
                    return;                
                }
            }
            
            // Parse child event path
            if (currentEvent != NULL)
            {
                EventPath.append(currentEvent->getPathExtension());
                json::Value childData = getData(EventPath);
                
                //console() << "EventPath "<< EventPath << std::endl;
                
                BOOST_FOREACH(EffectAttrMap::value_type i, currentEvent->getAttributes()) 
                {
                    parseAttr(childData, i.second, currentEvent);
                }            
            
                // Parse global parameters
                currentEvent->setEnabled(currentBlock["Enabled"].asBool());
            
                float startTime = currentBlock["StartTime"].asFloat();
            
                vector<json::Value> posValues = json::readValues(currentBlock, "Position");
                currentEvent->setEmitterPosition(Vec3f(posValues[0].asFloat(), posValues[1].asFloat(), posValues[2].asFloat()));       
            
                vector<json::Value> orientValues = json::readValues(currentBlock, "Orientation");
                Vec3f orientation = Vec3f(orientValues[0].asFloat(), orientValues[1].asFloat(), orientValues[2].asFloat());
            
                //addChild( EffectEventRef(currentEvent) );
                mEvents.push_back(currentEvent);
            }
        }
    }
    else {
        console() << "ERROR: Invalid Data."<< std::endl;
    }
}

void Effect::parseAttr(const json::Value data, EffectAttribute &attr, EffectEvent *currentEvent)
{
    //console() << "Attr Type is: " << attr.type << std::endl;
    
    boost::any currentValue;
    
    if (attr.mType == "Color")
    {
        vector<json::Value> values = json::readValues(data, attr.mName);
        currentValue = Color(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
    }
    else if (attr.mType == "Texture")
    {
        currentValue = data[attr.mName].asString();
    }
    else if (attr.mType == "Float")
    {
        currentValue = data[attr.mName].asFloat();
    }
    else if (attr.mType == "Vector3")
    {
        vector<json::Value> values = json::readValues(data, attr.mName);
        currentValue = Vec3f(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
    }
    else if (attr.mType == "Bool")
    {
        currentValue = data[attr.mName].asBool();
    }
    else
    {
        console() << "ERROR:  Unrecognized Attr Type"<< std::endl;
    }
    
    currentEvent->setAttribute(attr.mName, currentValue);
}

Effect::~Effect()
{
    delete mCamera; 
    
	for( vector<EffectEvent *>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        delete (*it);
    }
    
    mEvents.clear();
}

void Effect::setup()
{
    initializeData();
    
	for( vector<EffectEvent *>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
            (*it)->setup();
    }
    
    /*
    std::vector<EffectEventRef> effectChildren;
    std::copy(effectChildren.begin(), effectChildren.end(), std::back_inserter(mChildren));
    
    // update children
    BOOST_FOREACH(EffectEventRef effectChild, effectChildren) 
    {  
        if (effectChild->isEnabled()) 
        {
            effectChild->setup();
        }
    }
    */
}

void Effect::update()
{
}

void Effect::deepUpdate()
{
    if (mIsVisible) {
        // update self
        update();
        
        for( vector<EffectEvent *>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
        {
            if ((*it)->isEnabled())
                (*it)->update(*mCamera);
        }
        
        /*
        std::vector<EffectEventRef> effectChildren;
        std::copy(effectChildren.begin(), effectChildren.end(), std::back_inserter(mChildren));
        
        // update children
        BOOST_FOREACH(EffectEventRef effectChild, effectChildren) {  
            effectChild->update(*mCamera);
            // TODO:  maybe at some point an effect's children will have children, but not at the moment
            //child->deepUpdate();
        }
        */
    }
}

void Effect::draw()
{    
    for( vector<EffectEvent *>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it )
    {
        if ((*it)->isEnabled())
            (*it)->draw();
    }
}

void Effect::setCamera(ci::CameraPersp &camera)
{
    mCamera = &camera;
}