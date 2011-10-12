//
//  ParticleEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"

#include "cinder/Rand.h"
#include <vector>
#include <list>

using namespace ci;
using namespace std;

enum EmitModes {
    EMIT_BURST,
    EMIT_CONTINUOUS
};

// add new Emit Modes here
const boost::unordered_map<string, EmitModes> EMIT_MODES = boost::assign::map_list_of
    ("Burst", EMIT_BURST)
    ("Continuous", EMIT_CONTINUOUS);

enum BlendModes {
    BLEND_ALPHA,
    BLEND_ADDITIVE
};

// add new Blend Modes here
const boost::unordered_map<string, BlendModes> BLEND_MODES = boost::assign::map_list_of
    ("Alpha", BLEND_ALPHA)
    ("Additive", BLEND_ADDITIVE);
    
class ParticleEvent : public EffectEvent {

public:
    ParticleEvent();

    ~ParticleEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type, Member)
        registerAttribute("DiffuseTexture",   "Texture");
        registerAttribute("DiffuseColor",     "Color");
        registerAttribute("ParticleScale",    "Float");
        registerAttribute("EmitScale",        "Float");
        registerAttribute("Rate",             "Float");
        registerAttribute("InitialSpeed",     "Float");
        registerAttribute("RotationAngle",    "Float");
        registerAttribute("BlendMode",        "String");
        registerAttribute("Lifetime",         "Float");
        registerAttribute("ParticleLifetime", "Vector2");
        registerAttribute("EmitMode",         "String");
        registerAttribute("Alpha",            "Curve");
    }
    
    void setup();
    void update(const ci::CameraPersp &camera);
    void draw();
    void deepDraw(){}
    
private:
    void processAttributes();
    void addNewParticle();
    
    // user-defined
    // TODO these and others will need variance eventually (new type)
    float mRate;
    float mEmitScale;
    float mParticleScale;
    float mRotationAngle;
    float mInitialSpeed;
    int mEmitMode;
    Vec2f mParticleLifetime;  // value, variance
    string mBlendMode;
    
    floatCurve mAlphaCurve;
    float mCurrentAlpha;
    
    // for continuous emit mode
    float mPreviousElapsed;
    float mCurrentRate;  
    
    Color mDiffuseColor;
    gl::Texture mDiffuseTexture;
    
    //---------------------------------
    struct Particle {
        float scale;
        float rotation;
        float startTime;
        float lifetime;
        float maxLifetime;
        
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
    
    std::list<Particle> mParticles;
	VertexData *mVerts;
};