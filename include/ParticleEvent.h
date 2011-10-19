//
//  ParticleEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
#include "EmitVolume.h"

#include "cinder/Rand.h"
#include <vector>
#include <list>

using namespace ci;
using namespace std;

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
        registerAttribute("EmitScale",        "Vector3");
        registerAttribute("EmitVolumeType",   "String");
        registerAttribute("EmitMode",         "String");
        registerAttribute("EmitAngle",        "Vector2");
        
        registerAttribute("Alpha",            "Curve");
        registerAttribute("ParticleScale",    "Curve");
        
        registerAttribute("DiffuseTexture",   "Texture");
        registerAttribute("DiffuseColorR",    "Curve");
        registerAttribute("DiffuseColorG",    "Curve");
        registerAttribute("DiffuseColorB",    "Curve");
        registerAttribute("BlendMode",        "String");

        registerAttribute("InitialRotation",  "Vector2"); 
        registerAttribute("InitialSpeed",     "Vector2");
        registerAttribute("GlobalForce",      "Vector3");
        registerAttribute("DragForce",        "Vector3"); 
        registerAttribute("RotationSpeed",    "Vector2");  
        registerAttribute("InheritTransform", "Bool"); 

        //TODO curves also need up front variance (not per frame variance)
    }
    
    void setup();
    void update(const ci::CameraPersp &camera);
    void draw();
    void deepDraw(){}
    
private:
    struct Particle {
        float rotation;
        float rotationSpeed;
        float startTime;
        float lifetime;
        float maxLifetime;
        
        //TODO need to possibly store curves here with baked in variance.  eesh.  
        float alpha;
        float scale;
        float colorR;
        float colorG;
        float colorB;
        
        floatCurve alphaCurve;
        floatCurve scaleCurve;
        floatCurve colorRCurve;
        floatCurve colorGCurve;
        floatCurve colorBCurve;
        
        Vec3f position;  
        Vec3f velocity;
    };
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        ci::Vec4f color;
    };
    
    //-------------------------------------
    
    void processAttributes();
    floatCurve getNewCurve(AttributeCurvePoints &curvePoints);
    void addNewParticle();
    
    Vec3f getEmitDirection();
    void updateVelocity(Particle &currentParticle, float seconds);
    void enableBlendMode();
    void disableBlendMode();
    
    // user-defined attributes
    float mRate;
    Vec2f mParticleLifetime;  // value, variance
    EmitMode mEmitMode;
    Vec2f mEmitAngle;
        
    AttributeCurvePoints mAlphaCurve;
    AttributeCurvePoints mParticleScaleCurve;
    
    AttributeCurvePoints mDiffuseRedCurve;
    AttributeCurvePoints mDiffuseGreenCurve;
    AttributeCurvePoints mDiffuseBlueCurve;
    
    gl::Texture mDiffuseTexture;
    BlendMode mBlendMode;
    
    float mRotationAngle;
    Vec2f mInitialSpeed;     // value, variance
    Vec2f mInitialRotation;  // value, variance
    Vec2f mRotationSpeed;    // value, variance
    Vec3f mGlobalForce;  
    Vec3f mDragForce;
    
    bool mInheritTransform;
    
    EmissionVolume mEmissionVolume;
    //---------------------------------
    
    // for continuous emit mode
    float mPreviousElapsed;
    float mCurrentRate;  
    
	int mTotalVertices;
    int mPrevTotalVertices;
    
    std::list<Particle> mParticles;
	VertexData *mVerts;
};