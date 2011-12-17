//
//  EffectsManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
//#include "json/json.h"

#include "EffectsRenderer.h"
#include "AudioManager.h"
#include "json/json.h"
#include "EffectJson.h"
#include "EffectsState.h"
#include "Effect.h"
#include <list>

class EffectsManager;

typedef std::shared_ptr<EffectsManager> EffectsManagerRef;
typedef boost::unordered_map<std::string, Json::Value> EffectJsonMap;
typedef boost::unordered_map<std::string, EffectAttrMap> EventAttrMap;
typedef std::shared_ptr<ci::CameraPersp> CameraRef;

const ci::Vec3f KEYLIGHT_DIR = ci::Vec3f(1.0f, 0.0f, 0.5f);

class EffectsManager
{
public:

    static EffectsManagerRef create();
    
    ~EffectsManager(); 
    
	void setup(ci::Vec2i windowSize);
	void update();
	void draw();
    
    void setWindowSize( ci::Vec2i windowSize );
    
    void setBackgroundColor(ci::Color bgColor);
    
    void setCamera(CameraRef camera) { mCamera = camera; }
    
    void playEffectOnce(std::string effectName, Matrix44f transform=ci::Matrix44<float>::identity());
    EffectRef createEffect(std::string effectName, bool start=true, Matrix44f transform=ci::Matrix44<float>::identity());
    void destroyEffect(EffectRef effect, bool hardStop = false);
    
    void loadFEV(std::string filepath);
    
    EffectEventList initializeData(Json::Value data);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
protected:
    CameraRef mCamera;
    EffectsStateRef mState;
    EffectsRendererRef mRenderer;
    AudioManagerRef mAudioManager;
    
    std::list<EffectWeakRef> mEffects;
    std::list<EffectRef> mOneOffEffects;
    
    //cache of json resources
    EffectJsonMap mEffectsData;
    
    //cache of event attributes
    EventAttrMap mEventAttrs;
    
    ci::Vec2i mWindowSize;
    
private:

    EffectsManager();
};
