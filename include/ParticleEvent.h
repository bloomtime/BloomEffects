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
        registerAttribute("TileUV",           ATTR_VECTOR3);
        
        registerAttribute("Shader"        ,   ATTR_SHADER);
        registerAttribute("DiffuseTexture",   ATTR_TEXTURE);
        registerAttribute("DiffuseColorR",    ATTR_VECTOR3);
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
        registerAttribute("CameraAttached",   ATTR_BOOL);
        registerAttribute("ScreenSizeLOD",    ATTR_VECTOR2); 
    }
    
    void setup();
    void update();
    void draw();
    void deepDraw(){}
    
    void setTintColor( Vec3f color ) 
    { 
        //alpha will be handled elsewhere
        mTintColorAlpha = Vec4f(color, mTintColorAlpha.w); 
    }
    
    void setTintAlpha ( float alpha ) { mTintColorAlpha.w = alpha; }
    Vec2f getNormalizedScreenPos(Vec3f worldPos);
protected:

    struct Particle {
        float rotation;
        float rotationSpeed;
        float startTime;
        float lifetime;
        float maxLifetime;
        
        floatCurve alphaCurve;
        floatCurve scaleCurve;
        
        Vec2f tileUVLerp;
        Vec2f diffuseRedLerp;

        floatCurve colorGCurve;
        floatCurve colorBCurve;
                
        Vec3f position;  
        Vec3f velocity;
    };
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec4f color;
        ci::Vec2f texture;
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
    
    AttributeCurvePoints mDiffuseGreenCurve;
    AttributeCurvePoints mDiffuseBlueCurve;
    
    Vec3f mTileUVLerp;
    Vec3f mDiffuseRedLerp;
    
    bool mCameraAttached;
    
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
    
    Vec4f mTintColorAlpha;
    Vec2f mScreenSizeLOD;
    
    EmissionVolume mEmissionVolume;
    
    gl::GlslProg mShader;
    GLuint vtx_buffer;
    GLuint pos_handle, txc_handle, col_handle, tile_handle;
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