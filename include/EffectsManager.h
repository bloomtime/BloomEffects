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

class EffectsManager;

typedef std::shared_ptr<EffectsManager> EffectsManagerRef;

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
    
    CameraPersp* mCamera;
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
    
protected:

    std::list<EffectRef> mEffects;
    
    
private:

    EffectsManager();
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