//
//  PostEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"

#include "cinder/Rand.h"
#include "cinder/gl/GlslProg.h"
#include <vector>
#include <list>

using namespace ci;
using namespace std;

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
    
    void setup();
    void update();
    void draw();
    
protected:
    //-------------------------------------
    
    void processAttributes();
    floatCurve getNewCurve(AttributeCurvePoints &curvePoints);

    Vec2f mFadeTime;
    string mShader;
            
private: 

    PostEvent();
};