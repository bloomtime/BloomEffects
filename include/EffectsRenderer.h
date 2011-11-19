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

using namespace std;
using namespace ci;

class EffectsRenderer;

typedef std::shared_ptr<EffectsRenderer> EffectsRendererRef;

class EffectsRenderer
{
public:

    static EffectsRendererRef create();
    
    ~EffectsRenderer(); 
    
	void setup(EffectsStateRef fxState);
	void update(list<EffectRef> effects);
	void draw();
    
    void setBackgroundColor(Color bgColor) { mBGColor = bgColor; }
    
    //TODO need to cache here (maybe combine this with effect attribute)
    void setDefaultPostShader() { mCurrentPostShader = mDefaultPostShader; }
    void setPostShader(string shaderName);
    
protected:

    CameraOrtho mPostCamera;
    EffectsStateRef mState;

    gl::Fbo ca_read_fbo, ca_write_fbo;
    Vec2i fbo_size;

    Color mBGColor;
    
    string mPostShaderName;
    
    gl::GlslProg mBasicShader, mCurrentPostShader;
    gl::GlslProg mDefaultPostShader;
    
    GLuint vtx_handle, txc_handle;
    GLuint vtx_handle_post, txc_handle_post;
    
private:

    EffectsRenderer();
};