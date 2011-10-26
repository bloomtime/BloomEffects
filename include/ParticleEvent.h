//
//  ParticleEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
#include "EmitVolume.h"

#include "cinder/Rand.h"
#include "cinder/gl/GlslProg.h"
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

class ParticleEvent;

typedef std::shared_ptr<ParticleEvent> ParticleEventRef;

class ParticleEvent : public EffectEvent {

public:

    static ParticleEventRef create();

    ~ParticleEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type)
        registerAttribute("Rate",             ATTR_FLOAT);
        registerAttribute("Lifetime",         ATTR_FLOAT);
        registerAttribute("ParticleLifetime", ATTR_VECTOR2);
        registerAttribute("EmitScale",        ATTR_VECTOR3);
        registerAttribute("EmitVolumeType",   ATTR_STRING);
        registerAttribute("EmitMode",         ATTR_STRING);
        registerAttribute("EmitAngle",        ATTR_VECTOR2);
        
        registerAttribute("Alpha",            ATTR_CURVE);
        registerAttribute("ParticleScale",    ATTR_CURVE);
        registerAttribute("TileUV",           ATTR_CURVE);
        
        registerAttribute("Shader"        ,   ATTR_SHADER);
        registerAttribute("DiffuseTexture",   ATTR_TEXTURE);
        registerAttribute("DiffuseColorR",    ATTR_CURVE);
        registerAttribute("DiffuseColorG",    ATTR_CURVE);
        registerAttribute("DiffuseColorB",    ATTR_CURVE);
        registerAttribute("BlendMode",        ATTR_STRING);
        registerAttribute("TiledTexture",     ATTR_BOOL);
        registerAttribute("BlendTiles",       ATTR_BOOL);

        registerAttribute("InitialRotation",  ATTR_VECTOR2); 
        registerAttribute("InitialSpeed",     ATTR_VECTOR2);
        registerAttribute("GlobalForce",      ATTR_VECTOR3);
        registerAttribute("DragForce",        ATTR_VECTOR3); 
        registerAttribute("RotationSpeed",    ATTR_VECTOR2);  
        registerAttribute("InheritTransform", ATTR_BOOL); 

        //TODO curves also need up front variance (not per frame variance)
    }
    
    void setup();
    void update();
    void draw();
    void deepDraw(){}
    
protected:

    struct Particle {
        float rotation;
        float rotationSpeed;
        float startTime;
        float lifetime;
        float maxLifetime;
        
        float alpha;
        float scale;
        float colorR;
        float colorG;
        float colorB;
        float tileIndex;
        float tileMix;
        
        floatCurve alphaCurve;
        floatCurve scaleCurve;
        floatCurve colorRCurve;
        floatCurve colorGCurve;
        floatCurve colorBCurve;
        floatCurve tileUVCurve;
        
        Vec3f position;  
        Vec3f velocity;
    };
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        ci::Vec4f color;
        float tileIndex;
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
    
    AttributeCurvePoints mTileUVCurve;
    
    gl::Texture mDiffuseTexture;
    BlendMode mBlendMode;
    bool mTiledTexture;
    bool mBlendTiles;
    float mNumTiles;
    float mTileWidth;
    
    float mRotationAngle;
    Vec2f mInitialSpeed;     // value, variance
    Vec2f mInitialRotation;  // value, variance
    Vec2f mRotationSpeed;    // value, variance
    Vec3f mGlobalForce;  
    Vec3f mDragForce;
    
    EmissionVolume mEmissionVolume;
    
    gl::GlslProg mShader;
    GLuint vtx_handle, txc_handle, col_handle, tile_handle;
    //---------------------------------
    
    // for continuous emit mode
    float mPreviousElapsed;
    float mCurrentRate;  
    
	int mTotalVertices;
    int mPrevTotalVertices;
    
    std::list<Particle> mParticles;
	VertexData *mVerts;
    
private: 

    ParticleEvent();
};