#include "EffectsManager.h"

namespace bloom { namespace effects {

using namespace std;
using namespace ci;

const string FX_EXTENSION = ".effect.json";
EffectsManagerRef EffectsManager::create(RenderManagerRef renderMgr)
{
    return EffectsManagerRef( new EffectsManager(renderMgr) );
}

EffectsManager::EffectsManager(RenderManagerRef renderMgr)
{
    mRenderManager = renderMgr;
    mIsDeviceMaxSpec = bloom::isIpad2();
}

EffectsManager::~EffectsManager()
{
	for( list<EffectWeakRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        //(*it) = NULL;
        it = mEffects.erase(it);
    }
    
	for( list<EffectRef>::iterator it = mOneOffEffects.begin(); it != mOneOffEffects.end(); ++it )
    {
        //(*it) = NULL;
        it = mOneOffEffects.erase(it);
    }
    
    mEffectsData.clear();
    mEventAttrs.clear();
}

void EffectsManager::setup(Vec2i windowSize)
{
    mWindowSize = windowSize;
    
    mAudioManager = AudioManager::create();
    mAudioManager->setup();
    mAudioManager->setCamera(mCamera);
    
    //events will need to register separately later when they want to draw in different renderlayers, so disabling this 
    //mRenderManager->registerDraw(this, &EffectsManager::draw);
}

void EffectsManager::setWindowSize(Vec2i windowSize)
{
    mWindowSize = windowSize;
}

void EffectsManager::loadFEV(string filepath)
{
    if (mAudioManager)
        mAudioManager->loadFEV(filepath);
}

void EffectsManager::playEffectOnce(string effectName, Matrix44f transform)
{
    Json::Value data = getEffectData(effectName + FX_EXTENSION); // checks cache, loads if necessary
    
    EffectRef newEffect = Effect::create();
    newEffect->setCamera(mCamera);
    newEffect->setTransform(transform);
    newEffect->setup(initializeData(data), mWindowSize);
    
    mOneOffEffects.push_back(newEffect);
    newEffect->start();
}

EffectRef EffectsManager::createEffect(string effectName, bool start, Matrix44f transform)
{
    Json::Value data = getEffectData(effectName + FX_EXTENSION); // checks cache, loads if necessary
    
    EffectRef newEffect = Effect::create();
    newEffect->setCamera(mCamera);
    newEffect->setTransform(transform);
    newEffect->setup(initializeData(data), mWindowSize);
    
    mEffects.push_back(newEffect);
    
    if (start)
        newEffect->start();
        
    return newEffect;
}

bool EffectsManager::checkSpecEnabled(Json::Value currentBlock)
{
    SpecEnabledMode specMode = SPEC_MODE.at(currentBlock["SpecEnabled"].asString());
    
    switch (specMode) 
    {
        case SPEC_MIN:
        {
            return !mIsDeviceMaxSpec;
        }
        case SPEC_MAX:
        {
            return mIsDeviceMaxSpec;
        }
        default:
        {
            return true; 
        }
    }
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
            bool specEnabled = checkSpecEnabled(currentBlock);
            
            if (!enabled || !specEnabled)
                continue;
            
            if (eventType == "" || eventPath == "")
                break;
            
            //console() << "EVENT: " << EventPath << " - " << EventType << std::endl;
            
            ChildEventType ChildType = CHILD_EVENT_TYPES.at(eventType.c_str());
            EffectEventRef currentEvent;
            
            // handle the different types here
            switch (ChildType) 
            {
                case FASTPARTICLE_EVENT:
                {
                    currentEvent = FastParticleEvent::create();
                    // need to concatenate extension here
                    break;
                }
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
                case SOUND_EVENT:
                {
                    currentEvent = SoundEvent::create(mAudioManager);
                    break;
                }
                default:
                {
                    //nothing!
                    std::cout << "WARNING: Unrecognized EventType" << eventType << std::endl;
                    return effectEvents;                
                }
            }
            
            // Parse child event path
            if (currentEvent)
            {
                // Parse global parameters
                currentEvent->setEnabled(enabled);
                currentEvent->setRenderManager(mRenderManager);
                currentEvent->setKeyLightDir(KEYLIGHT_DIR);
                currentEvent->setToStartTime(currentBlock["StartTime"].asFloat());
            
                vector<Json::Value> posValues = effects::readVector(currentBlock, "Position");
                currentEvent->setSourcePosition(Vec3f(posValues[0].asFloat(), posValues[1].asFloat(), posValues[2].asFloat()));       
            
                vector<Json::Value> orientValues = effects::readVector(currentBlock, "Orientation");
                currentEvent->setSourceOrientation(Vec3f(orientValues[0].asFloat(), orientValues[1].asFloat(), orientValues[2].asFloat()));
                
                eventPath.append(currentEvent->getPathExtension());
                Json::Value childData = getEffectData( eventPath ); // checks cache, only loads if necessary
                
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
                    
                    for(EffectAttrMap::value_type &i : attrs) 
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
        std::cout << "ERROR: Invalid Data."<< std::endl;
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
        case ATTR_INT:
        {
            currentValue = data[attr.mName].asInt();
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
            std::cout << "ERROR:  Unrecognized Attr Type " << attr.mType << " for " << attr.mName << std::endl;
            break;               
        }
    }

    currentEvent->setAttribute(attr.mName, currentValue);
}


void EffectsManager::destroyEffect(EffectRef effect, bool hardStop)
{
	for( list<EffectWeakRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        EffectWeakRef current = (*it);
        
        if ( EffectRef currentEffect = current.lock()) 
        {
            if (currentEffect == effect)
            {
                currentEffect->stop(hardStop);
                it = mEffects.erase(it);
            }
        }
    }
}

void EffectsManager::update()
{
	for( list<EffectWeakRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        EffectWeakRef current = (*it);
        
        if ( EffectRef currentEffect = current.lock()) 
        {
            if (!currentEffect->isStopped())
            {
                currentEffect->update();
            }
        }
        else
        {
            it = mEffects.erase(it);
        }
    }
    
	for( list<EffectRef>::iterator it = mOneOffEffects.begin(); it != mOneOffEffects.end(); ++it )
    {
        EffectRef currentEffect = (*it);
        
        if (!currentEffect->isStopped())
        {
            currentEffect->update();
        }
        else
        {
            it = mOneOffEffects.erase(it);
        }
    }
    
    mAudioManager->update();
}

void EffectsManager::draw(bool enabled)
{  
    for( list<EffectWeakRef>::const_iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        EffectWeakRef current = (*it);
        
        if ( EffectRef currentEffect = current.lock()) 
        {
            if (!currentEffect->isStopped())
            {
                currentEffect->draw();
            }
        }
    }
    for( list<EffectRef>::const_iterator it = mOneOffEffects.begin(); it != mOneOffEffects.end(); ++it )
    {
        if (!(*it)->isStopped())
        {
            (*it)->draw();
        }
    }
}

}} // namespace bloom::effects

