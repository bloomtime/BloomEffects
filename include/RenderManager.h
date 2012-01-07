//
//  RenderManager.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//
//  TODO need to move out of BloomEffects to something else

#pragma once
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Function.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "Vbo.h"
#include <string>
#include <boost/unordered_map.hpp>

enum RenderLayer 
{
    LAYER_BACKGROUND,  // background 
    LAYER_SCENE,       // base scene rendering
    LAYER_EFFECTS,     // particle effects, etc
    LAYER_FOREGROUND,  // foreground
    LAYER_PREPOST,     // distortion effects, etc
    LAYER_POST,        // post filter 
    LAYER_UI,          // ui elements
    LAYER_INVALID      // nothing.  always last
};
    
class RenderManager;

typedef std::shared_ptr<RenderManager> RenderManagerRef;
typedef ci::CallbackMgr<void(bool)> RenderCallbacks;
typedef boost::unordered_map<RenderLayer, RenderCallbacks> RenderCallbacksByLayer;

const std::string DEFAULT_POST_SHADER = "defaultPost";

typedef std::function<void(void)> VoidFunc;

class RenderManager
{
public:

    static RenderManagerRef create(std::string defaultPost = DEFAULT_POST_SHADER);
    
    ~RenderManager(); 
    
	void setup(ci::Vec2i windowSize);
	void update();
	void draw();
    
    ci::gl::Texture getReadBuffer();
    
    void setWindowSize(ci::Vec2i windowSize);
    void setBackgroundColor(ci::Color bgColor) { mBGColor = bgColor; }
    
    //TODO need to cache here (maybe combine this with effect attribute)
    void setDefaultPostShader() { mCurrentPostShader = mDefaultPostShader; }
    void setPostShader(std::string shaderName);
    void setPostShaderAlpha(float alpha) { mPostShaderAlpha = alpha; }
    
    template<typename T>
    ci::CallbackId registerDraw( T *obj, void (T::*callback)(bool), RenderLayer layer=LAYER_SCENE )
    {
        ci::CallbackId cid = mCallbacks[layer].registerCb(std::bind( callback, obj, std::tr1::placeholders::_1 ));
        
        return cid;
    }
    
    void unregisterDraw(RenderLayer layer, ci::CallbackId cid)
    {
        mCallbacks[layer].unregisterCb(cid);
    } 
    
protected:

    ci::CameraOrtho mPostCamera;
    ci::Vec2i mWindowSize;

    ci::gl::Fbo ca_read_fbo, ca_write_fbo;
    ci::Vec2i fbo_size;

    ci::gl::VboRef mVBO;
    ci::gl::VboRef mPostVBO;
    
    ci::Color mBGColor;
    
    std::string mPostShaderName;
    
    float mPostShaderAlpha;
    
    ci::gl::GlslProg mBasicShader, mCurrentPostShader;
    ci::gl::GlslProg mDefaultPostShader;
    
    RenderCallbacksByLayer mCallbacks;
private:

    RenderManager(std::string defaultPost = DEFAULT_POST_SHADER);
};