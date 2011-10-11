//
//  ParticleEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"

#include "cinder/Rand.h"
#include <vector>

using namespace ci;
using namespace std;

class ParticleEvent : public EffectEvent {

public:
    ParticleEvent();

    ~ParticleEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type, Member)
        registerAttribute("DiffuseTexture", "Texture");
        registerAttribute("DiffuseColor",   "Color");
        registerAttribute("ParticleScale",  "Float");
        registerAttribute("EmitScale",      "Float");
        registerAttribute("Rate",           "Float");
        registerAttribute("InitialSpeed",   "Float");
        registerAttribute("RotationAngle",  "Float");
    }
    
    void setup();
    void update(const ci::CameraPersp &camera);
    void draw();
    void deepDraw();
    
private:
    void processAttributes();
    
    // user-defined
    // TODO these and others will need variance eventually (new type)
    float mRate;
    float mEmitScale;
    float mParticleScale;
    float mRotationAngle;
    float mInitialSpeed;
    Color mDiffuseColor;
    gl::Texture mDiffuseTexture;
    
    //---------------------------------
    struct Particle {
        float scale;
        float rotation;
        float lifetime;
        
        Vec3f position;
        Color color;
    };
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        ci::Vec4f color;
    };
    
	int mTotalVertices;
    int mPrevTotalVertices;
    
    std::vector<Particle> mParticles;
	VertexData *mVerts;
};