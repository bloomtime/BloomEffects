#include "EffectsManager.h"
#include <boost/foreach.hpp>

const string FX_EXTENSION = ".effect.json";
EffectsManagerRef EffectsManager::create()
{
    return EffectsManagerRef( new EffectsManager() );
}

EffectsManager::EffectsManager()
{
     mBGColor = Color(1.0f, 1.0f, 1.0f);
}

EffectsManager::~EffectsManager()
{
	for( list<EffectRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        //(*it) = NULL;
        it = mEffects.erase(it);
    }
    
    mEffectsData.clear();
    mEventAttrs.clear();
}

void EffectsManager::setup()
{
    mRenderer = EffectsRenderer::create();
    mRenderer->setup();
}


EffectRef EffectsManager::createEffect(string effectName, bool start, Matrix44f transform)
{
    Json::Value data;
    
    //TODO store all loaded json resources.  Later should store effect resources only, not the json
    if (mEffectsData.find(effectName) != mEffectsData.end())
    {
        data = mEffectsData.at(effectName);
    }
    else
    {
        data = effects::getJsonData(effectName + FX_EXTENSION);
        mEffectsData[effectName] = data;
    }
    
    EffectRef newEffect = Effect::create();
    newEffect->setCamera(mCamera);
    newEffect->setTransform(transform);
    newEffect->setup(initializeData(data));
    
    mEffects.push_back(newEffect);
    
    if (start)
        newEffect->start();
        
    return newEffect;
}

EffectEventList EffectsManager::initializeData(Json::Value data)
{
    list<EffectEventRef> effectEvents;
    
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
            
            ChildEventType ChildType = CHILD_EVENT_TYPES.at(eventType.c_str());
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
                case POST_EVENT:
                {
                    currentEvent = PostEvent::create();
                    // need to concatenate extension here
                    break;
                }
                default:
                {
                    //nothing!
                    console() << "WARNING: Unrecognized EventType" << eventType << std::endl;
                    return effectEvents;                
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
                
                EffectAttrMap attributes;
                
                //check if event attrs cached already, otherwise parse attributes
                if (mEventAttrs.find(eventPath) != mEventAttrs.end())
                {
                    attributes = mEventAttrs.at(eventPath);
                    currentEvent->setAttributes(attributes);
                }
                else
                {
                    EffectAttrMap attrs = currentEvent->getAttributes();
                    
                    BOOST_FOREACH(EffectAttrMap::value_type i, attrs) 
                    {
                        parseAttr(childData, i.second, currentEvent);
                    }         
                    
                    mEventAttrs[eventPath] = currentEvent->getAttributes();
                }
                
                //addChild( EffectEventRef(currentEvent) );
                
                effectEvents.push_back(currentEvent);
            }
        }
    }
    else {
        console() << "ERROR: Invalid Data."<< std::endl;
    }
    
    return effectEvents;
}

void EffectsManager::parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent)
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


void EffectsManager::destroyEffect(EffectRef effect, bool hardStop)
{
	for( list<EffectRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if ((*it) == effect)
        {
            (*it)->stop(hardStop);
            it = mEffects.erase(it);
        }
    }
}

void EffectsManager::update()
{
    mRenderer->update(mEffects);
    
	for( list<EffectRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if (!(*it)->isStopped())
        {
            (*it)->update();
        }
        else
        {
            it = mEffects.erase(it);
        }
    }
}

void EffectsManager::draw()
{  
    mRenderer->draw();
}

