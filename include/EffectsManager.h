//
//  EffectsManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <list>
#include <json/json.h>

#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

#include "RenderManager.h"
#include "AudioManager.h"
#include "EffectJson.h"
#include "Effect.h"
#include "Device.h"

namespace bloom { namespace effects {

class EffectsManager;

typedef std::shared_ptr<EffectsManager> EffectsManagerRef;
typedef boost::unordered_map<std::string, Json::Value> EffectJsonMap;
typedef boost::unordered_map<std::string, EffectAttrMap> EventAttrMap;
typedef std::shared_ptr<ci::CameraPersp> CameraRef;

enum SpecEnabledMode 
{
    SPEC_ALL,
    SPEC_MAX,
    SPEC_MIN
};

const boost::unordered_map<std::string, SpecEnabledMode> SPEC_MODE = boost::assign::map_list_of
    ("ALL", SPEC_ALL)
    ("MAX_ONLY", SPEC_MAX)
    ("MIN_ONLY", SPEC_MIN);
    
const ci::Vec3f KEYLIGHT_DIR = ci::Vec3f(1.0f, 0.0f, 0.5f);

class EffectsManager
{
public:

    static EffectsManagerRef create(RenderManagerRef renderMgr);
    
    ~EffectsManager(); 
    
	void setup(ci::Vec2i windowSize);
	void update();
	void draw(bool enabled=true);
    
    void setWindowSize( ci::Vec2i windowSize );
    
    void setCamera(CameraRef camera) { mCamera = camera; }
    
    void playEffectOnce(std::string effectName, Matrix44f transform=ci::Matrix44<float>::identity());
    EffectRef createEffect(std::string effectName, bool start=true, Matrix44f transform=ci::Matrix44<float>::identity());
    void destroyEffect(EffectRef effect, bool hardStop = false);
    
    void loadFEV(std::string filepath);
    bool checkSpecEnabled(Json::Value currentBlock);
    
    EffectEventList initializeData(Json::Value data);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
    void setEffectData( std::string effectFileName, Json::Value data )
    {
        mEffectsData[effectFileName] = data;
    }
    
    Json::Value getEffectData( std::string effectFileName )
    {
        if (mEffectsData.find(effectFileName) != mEffectsData.end())
        {
            return mEffectsData.at(effectFileName);
        }
        else
        {
            Json::Value data = effects::getJsonData(effectFileName);
            mEffectsData[effectFileName] = data;
            return data;
        }
    }
    
protected:
    CameraRef mCamera;
    
    RenderManagerRef mRenderManager;
    AudioManagerRef mAudioManager;
    
    std::list<EffectWeakRef> mEffects;
    std::list<EffectRef> mOneOffEffects;
    
    //cache of json resources
    EffectJsonMap mEffectsData;
    
    //cache of event attributes
    EventAttrMap mEventAttrs;
    
    ci::Vec2i mWindowSize;
    bool mIsDeviceMaxSpec;
    
private:

    EffectsManager(RenderManagerRef renderMgr);
};

}} // namespace bloom::effects
