//
//  EffectsRenderer.h
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

class EffectsRenderer;

typedef std::shared_ptr<EffectsRenderer> EffectsRendererRef;

class EffectsRenderer
{
public:

    static EffectsRendererRef create();
    
    ~EffectsRenderer(); 
    
	void setup(EffectsStateRef fxState, ci::Vec2f windowSize);
	void update(list<EffectRef> effects);
	void draw();
    
    void setWindowSize(Vec2f windowSize);

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

    EffectsRenderer();
};