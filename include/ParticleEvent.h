//
//  ParticleEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
#include "ParticleModes.h"
#include "EmitVolume.h"

#include "cinder/gl/GlslProg.h"
#include <vector>
#include <list>

namespace bloom { namespace effects {

class ParticleEvent;

typedef std::shared_ptr<ParticleEvent> ParticleEventRef;

class ParticleEvent : public EffectEvent {

public:

    static ParticleEventRef create();

    ~ParticleEvent();
    
    void registerAttributes() 
    {
        // editable attributes (Name, Type)
        registerAttribute("Rate",                ATTR_FLOAT);
        registerAttribute("Lifetime",            ATTR_FLOAT);
        registerAttribute("ParticleLifetime",    ATTR_VECTOR2);
        registerAttribute("EmitScale",           ATTR_VECTOR3);
        registerAttribute("EmitVolumeType",      ATTR_STRING);
        registerAttribute("EmitMode",            ATTR_STRING);
        registerAttribute("EmitAngle",           ATTR_VECTOR2);
        
        registerAttribute("Alpha",               ATTR_CURVE);
        registerAttribute("ParticleScale",       ATTR_CURVE);
        registerAttribute("TileUV",              ATTR_VECTOR3);
        
        registerAttribute("RenderLayer",         ATTR_STRING);
        registerAttribute("Shader",              ATTR_SHADER);
        registerAttribute("DiffuseTexture",      ATTR_TEXTURE);
        registerAttribute("DiffuseTextureID",    ATTR_INT);
        registerAttribute("DiffuseSampleMode",   ATTR_STRING);
        registerAttribute("SecondaryTexture",    ATTR_TEXTURE);
        registerAttribute("Texture2SampleMode",  ATTR_STRING);
        registerAttribute("DiffuseColorR",       ATTR_VECTOR3);
        registerAttribute("DiffuseColorG",       ATTR_VECTOR3);
        registerAttribute("DiffuseColorB",       ATTR_VECTOR3);
        registerAttribute("BlendMode",           ATTR_STRING);
        registerAttribute("TiledTexture",        ATTR_BOOL);
        registerAttribute("BlendTiles",          ATTR_BOOL);
        registerAttribute("Texture2Mode",        ATTR_STRING);

        registerAttribute("InitialRotation",     ATTR_VECTOR2); 
        registerAttribute("InitialSpeed",        ATTR_VECTOR2);
        registerAttribute("GlobalForce",         ATTR_VECTOR3);
        registerAttribute("DragForce",           ATTR_VECTOR3); 
        registerAttribute("RotationSpeed",       ATTR_VECTOR2);  
        registerAttribute("InheritTransform",    ATTR_BOOL);
        registerAttribute("FacingMode",          ATTR_STRING);
        registerAttribute("CameraAttached",      ATTR_BOOL);
        registerAttribute("ScreenSizeLOD",       ATTR_VECTOR2); 
    }
    
    void setup(ci::Vec2f windowSize);
    void update();
    void draw(bool enabled=true);
    void deepDraw(){}
    
    void setTintColor( ci::Vec3f color ) 
    { 
        //alpha will be handled elsewhere
        mTintColorAlpha = ci::Vec4f(color, mTintColorAlpha.w); 
    }
    
    void setTexture( ci::gl::Texture texture, int ID);
    
    void setTintAlpha ( float alpha ) { mTintColorAlpha.w = alpha; }
    ci::Vec2f getNormalizedScreenPos(ci::Vec3f worldPos);
    
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
        
        ci::Vec2f tileUVLerp;
        ci::Vec2f diffuseRedLerp;
        ci::Vec2f diffuseGreenLerp;
        ci::Vec2f diffuseBlueLerp;
                
        ci::Vec3f position;  
        ci::Vec3f velocity;
    };

    //-------------------------------------
    
    void processAttributes();
    floatCurve getNewCurve(AttributeCurvePoints &curvePoints);
    void addNewParticle();
    
    ci::Vec3f getEmitDirection();
    void updateVelocity(Particle &currentParticle, float seconds);
    void enableBlendMode();
    void disableBlendMode();
    
    // user-defined attributes
    float mRate;
    ci::Vec2f mParticleLifetime;  // value, variance
    EmitMode mEmitMode;
    FaceMode mFaceMode;
    ci::Vec2f mEmitAngle;
        
    AttributeCurvePoints mAlphaCurve;
    AttributeCurvePoints mParticleScaleCurve;
    
    ci::Vec3f mTileUVLerp;
    ci::Vec3f mDiffuseRedLerp;
    ci::Vec3f mDiffuseGreenLerp;
    ci::Vec3f mDiffuseBlueLerp;
    
    bool mCameraAttached;
    
    ci::gl::Texture mDiffuseTexture;
    ci::gl::Texture mSecondaryTexture;
    
    int mDiffuseTextureID;
    
    BlendMode mBlendMode;
    Texture2Mode mTexture2Mode;
    bool mTiledTexture;
    bool mBlendTiles;
    float mNumTiles;
    float mTileWidth;
    
    float mRotationAngle;
    ci::Vec2f mInitialSpeed;     // value, variance
    ci::Vec2f mInitialRotation;  // value, variance
    ci::Vec2f mRotationSpeed;    // value, variance
    ci::Vec3f mGlobalForce;  
    ci::Vec3f mDragForce;
    
    ci::Vec4f mTintColorAlpha;
    ci::Vec2f mScreenSizeLOD;
    float mTimeElapsed;
    
    EmissionVolume mEmissionVolume;
    
    ci::gl::GlslProg mShader;
    RenderLayer mRenderLayer;
    //---------------------------------
    
    // for continuous emit mode
    float mPreviousElapsed;
    float mCurrentRate;  
    
    int mTotalVertices;
    
    float mWindowWidth;
    float mWindowHeight;
    
    std::list<Particle> mParticles;
    
    ci::gl::VboRef mTrianglesVBO;
    
private: 

    ParticleEvent();
};

}} // namespace bloom::effects
