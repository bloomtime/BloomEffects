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

#include "Effect.h"

using namespace std;
using namespace ci;

//DEBUG
//using namespace ci::app;

class EffectsRenderer;

typedef std::shared_ptr<EffectsRenderer> EffectsRendererRef;

class EffectsRenderer
{
public:

    static EffectsRendererRef create();
    
    ~EffectsRenderer(); 
    
	void setup();
	void update(list<EffectRef> effects);
	void draw();
    
    void setBackgroundColor(Color bgColor) { mBGColor = bgColor; }
    void setPostShader(string shaderName);
    void setPostShaderAlpha(float alpha);
        
protected:

    CameraOrtho mPostCamera;

    gl::Fbo ca_read_fbo, ca_write_fbo;
    Vec2i fbo_size;

    Color mBGColor;
    string mPostShaderName;
    float mPostShaderAlpha;
    
    gl::GlslProg prog, prog_post;
    
    GLuint vtx_handle, txc_handle;
    GLuint vtx_handle_post, txc_handle_post;
    
private:

    EffectsRenderer();
};