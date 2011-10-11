//
//  ParticleEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "ParticleEvent.h"


//TODO DEBUG only
#include "cinder/app/App.h"
using namespace ci;
using namespace ci::app;

const string PATH_EXTENSION = ".particle.json";

ParticleEvent::ParticleEvent():
    mRate(1.0f),
    mEmitScale(1.0f),
    mParticleScale(1.0f),
    mDiffuseColor(Color( 1.0f, 1.0f, 1.0f )),
    mTotalVertices(0),
    mPrevTotalVertices(0)
{ 
    mExtension = PATH_EXTENSION;
    registerAttributes(); 
}
    
ParticleEvent::~ParticleEvent()
{
    if (mVerts != NULL)	{
        delete[] mVerts;
        mVerts = NULL;
    }
    
    mParticles.clear();
}

void ParticleEvent::processAttributes()
{
    // if can associate these member vars with the attr name that would be sweet
    mDiffuseTexture = getTexture(mAttributes.at("DiffuseTexture").value);
    mDiffuseColor = getColor(mAttributes.at("DiffuseColor").value);
    mParticleScale = getFloat(mAttributes.at("ParticleScale").value);
    mEmitScale = getFloat(mAttributes.at("EmitScale").value);
    mRate = getFloat(mAttributes.at("Rate").value);
    mInitialSpeed = getFloat(mAttributes.at("InitialSpeed").value);
    mRotationAngle = getFloat(mAttributes.at("RotationAngle").value);
}

void ParticleEvent::setup()
{
    processAttributes();
    
    mVerts = NULL;
    mPrevTotalVertices = -1;
    
    mParticles.clear();
    
    // only works for burst mode right now
    for (int i=0; i<mRate; i++)
    {
        Particle newParticle;
        newParticle.scale = mParticleScale;
        newParticle.rotation = 0.0f;
        newParticle.lifetime = 1.0f;
        newParticle.position = mEmitterPosition + Rand::randVec3f() * mEmitScale;
        newParticle.color = mDiffuseColor;
        mParticles.push_back(newParticle);
    }
}


void ParticleEvent::update(const ci::CameraPersp &camera)
{
    Vec3f bbRight, bbUp;
    camera.getBillboardVectors( &bbRight, &bbUp );   
    
	mTotalVertices = mParticles.size() * 6;
	
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL) {
            delete[] mVerts; 
            mVerts = NULL;
        }
        if (mTotalVertices > 0) {
            mVerts = new VertexData[mTotalVertices];
            mPrevTotalVertices = mTotalVertices;
        }
    }
        
	int vIndex = 0;
	
	for( vector<Particle>::const_iterator it = mParticles.begin(); it != mParticles.end(); ++it ){
		
        Vec3f pos = (*it).position;
        Color c = (*it).color;
        float scale = (*it).scale;
		Vec4f col = Vec4f(c.r, c.g, c.b, 1.0);
    
        Vec3f right			= bbRight * scale;
        Vec3f up			= bbUp * scale;
        
        Vec3f p1			= pos - right - up;
        Vec3f p2			= pos + right - up;
        Vec3f p3			= pos - right + up;
        Vec3f p4			= pos + right + up;
        
        mVerts[vIndex].vertex  = p1;
        mVerts[vIndex].texture = Vec2f(0.0f,0.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p4;
        mVerts[vIndex].texture = Vec2f(1.0f,1.0f);
        mVerts[vIndex].color   = col;
        vIndex++;        
        
	}    
}

void ParticleEvent::draw()
{    
    // console() << "drawing" << std::endl;
    //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ONE_MINUS_ALPHA);
    //glEnable(GL_BLEND);
    gl::enableAlphaBlending();
    //gl::enableAdditiveBlending();
    
	mDiffuseTexture.enableAndBind();
    
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
    
	glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), mVerts );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );
	glColorPointer( 4, GL_FLOAT, sizeof(VertexData), &mVerts[0].color );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
    
	mDiffuseTexture.disable();
    gl::disableAlphaBlending();
    //gl::disableAdditiveBlending();

    // and then any children will be draw after this
}
