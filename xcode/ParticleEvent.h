//
//  ParticleEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/Function.h"
#include "cinder/Matrix.h"
#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

using namespace ci;

class ParticleEvent {

public:
    ParticleEvent():
        mScale(1.0f),
        mPosition(Vec3f( 0.0f, 0.0f, 0.0f ))
    {}
    
    ParticleEvent(float scale, Vec3f position): 
        mScale(scale),
        mPosition(position)
    {}
    
    ~ParticleEvent() {}
    
    void update();
    void draw();
    
    void setTransform( const ci::Matrix44f &transform ) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
private:
    
    float mScale;
    Vec3f mPosition;
    
    ci::Matrix44f mTransform;
    gl::Texture mDiffuseTexture;
};