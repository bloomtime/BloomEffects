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

enum EmitVolume {
    VOLUME_SPHERE,
    VOLUME_BOX,
    VOLUME_CYLINDER
};

// add new Emit Modes here
const boost::unordered_map<string, EmitVolume> EMIT_VOLUMES = boost::assign::map_list_of
    ("Sphere", VOLUME_SPHERE)
    ("Box", VOLUME_BOX)
    ("Cylinder", VOLUME_CYLINDER);

enum EmitMode {
    EMIT_BURST,
    EMIT_CONTINUOUS
};

// add new Emit Modes here
const boost::unordered_map<string, EmitMode> EMIT_MODES = boost::assign::map_list_of
    ("Burst", EMIT_BURST)
    ("Continuous", EMIT_CONTINUOUS);

enum BlendMode {
    BLEND_ALPHA,
    BLEND_ADDITIVE,
    BLEND_OPAQUE
};

// add new Blend Modes here
const boost::unordered_map<string, BlendMode> BLEND_MODES = boost::assign::map_list_of
    ("AlphaBlend", BLEND_ALPHA)
    ("AdditiveBlend", BLEND_ADDITIVE)
    ("OpaqueBlend", BLEND_OPAQUE);
    
class ParticleEvent : public EffectEvent {

public:
    ParticleEvent();

    ~ParticleEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type)
        registerAttribute("Rate",             "Float");
        registerAttribute("Lifetime",         "Float");
        registerAttribute("ParticleLifetime", "Vector2");
        registerAttribute("EmitScale",        "Float");
        registerAttribute("EmitMode",         "String");
        
        registerAttribute("Alpha",            "Curve");
        registerAttribute("ParticleScale",    "Curve");
        
        registerAttribute("DiffuseTexture",   "Texture");
        registerAttribute("DiffuseColor",     "Color");
        registerAttribute("BlendMode",        "String");

        registerAttribute("InitialRotation",  "Vector2"); 
        registerAttribute("InitialSpeed",     "Vector2");
        registerAttribute("GlobalForce",      "Vector3");
        registerAttribute("DragForce",        "Vector3"); 
        registerAttribute("RotationSpeed",    "Vector2");   
        
        //TODO attrs not hooked up yet
        //registerAttribute("RotationSpeed",    "Vector2");
        //registerAttribute("EmitAngle",        "Vector2");
        //TODO curves also need up front variance (not per frame variance)
    }
    
    void setup();
    void update(const ci::CameraPersp &camera);
    void draw();
    void deepDraw(){}
    
private:
    struct Particle {
        float scale;
        float rotation;
        float rotationSpeed;
        float startTime;
        float lifetime;
        float maxLifetime;
        float alpha;
        
        Vec3f position;
        Color color;
        
        Vec3f velocity;
    };
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        ci::Vec4f color;
    };
    
    //-------------------------------------
    
    void processAttributes();
    void addNewParticle();
    
    Vec3f getEmitDirection();
    void updateVelocity(Particle &currentParticle, float seconds);
    void enableBlendMode();
    void disableBlendMode();
    
    // user-defined attributes
    float mRate;
    Vec2f mParticleLifetime;  // value, variance
    float mEmitScale;
    EmitMode mEmitMode;
    EmitVolume mEmitVolume;
    Vec2f mEmitAngle;
        
    floatCurve mAlphaCurve;
    floatCurve mParticleScaleCurve;
    
    Color mDiffuseColor;
    gl::Texture mDiffuseTexture;
    BlendMode mBlendMode;
    
    float mRotationAngle;
    Vec2f mInitialSpeed;     // value, variance
    Vec2f mInitialRotation;  // value, variance
    Vec2f mRotationSpeed;    // value, variance
    Vec3f mGlobalForce;  
    Vec3f mDragForce;
    
    //---------------------------------
    
    // for continuous emit mode
    float mPreviousElapsed;
    float mCurrentRate;  
    
	int mTotalVertices;
    int mPrevTotalVertices;
    
    std::list<Particle> mParticles;
	VertexData *mVerts;
};