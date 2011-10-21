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

#include "Effect.h"
#include <list>

using namespace std;
using namespace ci;
using namespace ci::app;

class EffectsManager
{
public:
    EffectsManager();
    ~EffectsManager(); 
    
	void setup();
	void update();
	void draw();
    
    void setCamera(ci::CameraPersp *camera) { mCamera = camera; }
    void createEffect(string effectName, bool start=true);
    void stopEffect(bool hardStop = false);
    
    //STUB
    Json::Value mData;
    
    Effect* mTestEffect;
    
    CameraPersp* mCamera;
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
    
protected:

    std::list<Effect *> mEffects;
};

//TODO
class EffectsRenderer 
{
public:
    EffectsRenderer() {}
    ~EffectsRenderer() {}    

protected:

    GLuint m_framebuffer;
    GLuint m_renderbuffer;

};