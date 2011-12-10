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

enum Texture2Mode {
    TEX2_NONE,
    TEX2_NORMAL,
    TEX2_DISTORT
};

// add new Texture 2 Modes here
const boost::unordered_map<string, Texture2Mode> TEXTURE2_MODES = boost::assign::map_list_of
    ("None", TEX2_NONE)
    ("Normal", TEX2_NORMAL)
    ("Distort", TEX2_DISTORT);
    
    
enum FaceMode {
    FACE_BILLBOARD,
    FACE_AXIS_X,
    FACE_AXIS_Y,
    FACE_AXIS_Z,
};

// add new Face Modes here
const boost::unordered_map<string, FaceMode> FACE_MODES = boost::assign::map_list_of
    ("Billboard", FACE_BILLBOARD)
    ("AxisX", FACE_AXIS_X)
    ("AxisY", FACE_AXIS_Y)
    ("AxisZ", FACE_AXIS_Z);
    
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
        registerAttribute("SecondaryTexture", ATTR_TEXTURE);
        registerAttribute("DiffuseColorR",    ATTR_VECTOR3);
        registerAttribute("DiffuseColorG",    ATTR_VECTOR3);
        registerAttribute("DiffuseColorB",    ATTR_VECTOR3);
        registerAttribute("BlendMode",        ATTR_STRING);
        registerAttribute("TiledTexture",     ATTR_BOOL);
        registerAttribute("BlendTiles",       ATTR_BOOL);
        registerAttribute("Texture2Mode",     ATTR_STRING);

        registerAttribute("InitialRotation",  ATTR_VECTOR2); 
        registerAttribute("InitialSpeed",     ATTR_VECTOR2);
        registerAttribute("GlobalForce",      ATTR_VECTOR3);
        registerAttribute("DragForce",        ATTR_VECTOR3); 
        registerAttribute("RotationSpeed",    ATTR_VECTOR2);  
        registerAttribute("InheritTransform", ATTR_BOOL);
        registerAttribute("FacingMode",       ATTR_STRING);
        registerAttribute("CameraAttached",   ATTR_BOOL);
        registerAttribute("ScreenSizeLOD",    ATTR_VECTOR2); 
    }
    
    void setup(Vec2f windowSize);
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
    
    void setWindowDimensions ( float width, float height ) 
    {
        mWindowWidth = width;
        mWindowHeight = height;
    }
    
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
        Vec2f diffuseGreenLerp;
        Vec2f diffuseBlueLerp;
                
        Vec3f position;  
        Vec3f velocity;
    };
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec4f color;
        ci::Vec3f normal;
        ci::Vec3f tangent;
        ci::Vec3f bitangent;
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
    FaceMode mFaceMode;
    Vec2f mEmitAngle;
        
    AttributeCurvePoints mAlphaCurve;
    AttributeCurvePoints mParticleScaleCurve;
    
    Vec3f mTileUVLerp;
    Vec3f mDiffuseRedLerp;
    Vec3f mDiffuseGreenLerp;
    Vec3f mDiffuseBlueLerp;
    
    bool mCameraAttached;
    
    gl::Texture mDiffuseTexture;
    gl::Texture mSecondaryTexture;
    
    BlendMode mBlendMode;
    Texture2Mode mTexture2Mode;
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
    float mTimeElapsed;
    
    EmissionVolume mEmissionVolume;
    
    gl::GlslProg mShader;
    GLuint vtx_buffer;
    GLuint pos_handle, txc_handle, norm_handle, tan_handle, bitan_handle, col_handle, tile_handle;
    //---------------------------------
    
    // for continuous emit mode
    float mPreviousElapsed;
    float mCurrentRate;  
    
	int mTotalVertices;
    int mPrevTotalVertices;
    
    float mWindowWidth;
    float mWindowHeight;
    
    std::list<Particle> mParticles;
	VertexData *mVerts;
    
private: 

    ParticleEvent();
};