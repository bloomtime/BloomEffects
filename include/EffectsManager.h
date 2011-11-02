//
//  EffectsManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
//#include "json/json.h"

#include "json/json.h"
#include "EffectJson.h"
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

class EffectsManager
{
public:

    static EffectsManagerRef create();
    
    ~EffectsManager(); 
    
	void setup();
	void update();
	void draw();
    
    void setCamera(ci::CameraPersp *camera) { mCamera = camera; }
    EffectRef createEffect(string effectName, bool start=true);
    void destroyEffect(EffectRef effect, bool hardStop = false);
    
    EffectEventList initializeData(Json::Value data);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
    CameraPersp* mCamera;
    
    //TODO not needed yet
    //GLuint m_framebuffer;
    //GLuint m_renderbuffer;
    
protected:

    std::list<EffectRef> mEffects;
    
    //cache of json resources
    EffectJsonMap mEffectsData;
    
    //cache of event attributes
    EventAttrMap mEventAttrs;
    
private:

    EffectsManager();
};

//TODO STUB
class EffectsRenderer 
{
public:
    EffectsRenderer() {}
    ~EffectsRenderer() {}    

protected:

    GLuint m_framebuffer;
    GLuint m_renderbuffer;

};