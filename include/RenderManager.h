//
//  RenderManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

#include "Effect.h"
#include "EffectsState.h"

enum RenderLayer 
{
    LAYER_SCENE,    // base scene rendering
    LAYER_EFFECTS,  // particle effects, etc
    LAYER_PREPOST,  // distortion effects, etc
    LAYER_POST,     // post filter 
    LAYER_UI,       // ui elements
};


class RenderManager;

typedef std::shared_ptr<RenderManager> RenderManagerRef;

class RenderManager
{
public:

    static RenderManagerRef create();
    
    ~RenderManager(); 
    
	void setup(EffectsStateRef fxState, ci::Vec2i windowSize);
	void update(list<EffectWeakRef> effects, list<EffectRef> oneOffEffects);
	void draw();
    
    void setWindowSize(Vec2i windowSize);
    void setBackgroundColor(ci::Color bgColor) { mBGColor = bgColor; }
    
    //TODO need to cache here (maybe combine this with effect attribute)
    void setDefaultPostShader() { mCurrentPostShader = mDefaultPostShader; }
    void setPostShader(std::string shaderName);
    
protected:

    ci::CameraOrtho mPostCamera;
    EffectsStateRef mState;

    gl::Fbo ca_read_fbo, ca_write_fbo;
    ci::Vec2i fbo_size;

    ci::Color mBGColor;
    
    std::string mPostShaderName;
    
    ci::gl::GlslProg mBasicShader, mCurrentPostShader;
    ci::gl::GlslProg mDefaultPostShader;
    
    GLuint vtx_handle, txc_handle;
    GLuint vtx_handle_post, txc_handle_post;
    
private:

    RenderManager();
};