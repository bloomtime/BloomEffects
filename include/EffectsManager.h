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

#include "json/json.h"
#include "EffectJson.h"
#include "Effect.h"
#include "EffectsRenderer.h"
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

class EffectsManager
{
public:

    static EffectsManagerRef create();
    
    ~EffectsManager(); 
    
	void setup();
	void update();
	void draw();
    
    void setBackgroundColor(Color bgColor) { mBGColor = bgColor; }
    
    void setCamera(CameraRef camera) { mCamera = camera; }
    EffectRef createEffect(string effectName, bool start=true, Matrix44f transform=cinder::Matrix44<float>::identity());
    void destroyEffect(EffectRef effect, bool hardStop = false);
    
    EffectEventList initializeData(Json::Value data);
    void parseAttr(const Json::Value data, EffectAttribute &attr, EffectEventRef currentEvent);
    
    CameraRef mCamera;
    
    CameraOrtho postCamera;
        
    gl::Fbo ca_read_fbo, ca_write_fbo;
    Vec2i fbo_size;

    Color mBGColor;

    gl::GlslProg prog, prog_post;
    
    GLuint mSceneTex;
    GLuint vtx_handle, txc_handle;
    GLuint vtx_handle_post, txc_handle_post;
    
protected:
    EffectsRendererRef mRenderer;
    
    std::list<EffectRef> mEffects;
    
    //cache of json resources
    EffectJsonMap mEffectsData;
    
    //cache of event attributes
    EventAttrMap mEventAttrs;
    
private:

    EffectsManager();
};
