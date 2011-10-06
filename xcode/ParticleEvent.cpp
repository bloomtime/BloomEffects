//
//  ParticleEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "ParticleEvent.h"

void ParticleEvent::update()
{
    Matrix44f mat = getTransform();
    mat.setToIdentity();
    mat.rotate( Vec3f::zAxis(), 0.0f );
    mat.translate( mPosition );
    setTransform(mat);
}

void ParticleEvent::draw()
{    
    glPushMatrix();
    glMultMatrixf(mTransform); // FIXME only push/mult/pop if mTransform isn't identity
    // Matrix is already applied so we can draw at origin
    gl::color( Color::white() ); 
    //gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
    //gl::drawSolidCircle( Vec2f::zero(), mPlanetRadius );
    gl::drawSolidCircle( Vec2f::zero(), mScale );
    glPopMatrix();
  
    
    // and then any children will be draw after this
}
