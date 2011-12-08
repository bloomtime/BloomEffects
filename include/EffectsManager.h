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

using namespace std;
using namespace ci;

//DEBUG
//using namespace ci::app;

class EffectsManager;

typedef std::shared_ptr<EffectsManager> EffectsManagerRef;
typedef boost::unordered_map<string, Json::Value> EffectJsonMap;
typedef boost::unordered_map<string, EffectAttrMap> EventAttrMap;
typedef std::shared_ptr<CameraPersp> CameraRef;

const Vec3f KEYLIGHT_DIR = Vec3f(1.0f, 0.0f, 0.0f);

class EffectsManager
{
public:

    static EffectsManagerRef create();
    
    ~EffectsManager(); 
    
	void setup(Vec2f windowSize);
	void update();
	void draw();
    
    void setWindowSize( Vec2f windowSize );
    
    void setBackgroundColor(Color bgColor);
    
    void setCamera(CameraRef camera) { mCamera = camera; }
    EffectRef createEffect(string effectName, bool start=true, Matrix44f transform=cinder::Matrix44<float>::identity());
    void destroyEffect(EffectRef effect, bool hardStop = false);
    
    void loadFEV(string filepath);
    
    EffectEventList initializeData(Json::Value data);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
protected:
    CameraRef mCamera;
    EffectsStateRef mState;
    EffectsRendererRef mRenderer;
    AudioManagerRef mAudioManager;
    
    std::list<EffectRef> mEffects;
    
    //cache of json resources
    EffectJsonMap mEffectsData;
    
    //cache of event attributes
    EventAttrMap mEventAttrs;
    
    ci::Vec2f mWindowSize;
    
private:

    EffectsManager();
};
