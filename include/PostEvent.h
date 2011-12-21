//
//  PostEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include "EffectEvent.h"
#include "cinder/gl/GlslProg.h"
#include <vector>
#include <list>

class PostEvent;

typedef std::shared_ptr<PostEvent> PostEventRef;

class PostEvent : public EffectEvent {

public:

    static PostEventRef create();

    ~PostEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type)
        registerAttribute("Lifetime",         ATTR_FLOAT);
        registerAttribute("FadeTime",         ATTR_VECTOR2);
        registerAttribute("Shader",           ATTR_STRING);
    }
    
    void setup(ci::Vec2f windowSize);
    void update();
    void draw();
    
protected:
    //-------------------------------------
    
    void processAttributes();

    ci::Vec2f mFadeTime;
    std::string mShaderName;
    std::string mPreviousShader;
    
    float mFadeStartTime; // for infinite duration soft stop
            
private: 

    PostEvent();
};