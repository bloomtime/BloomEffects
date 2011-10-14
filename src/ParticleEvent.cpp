//
//  ParticleEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "ParticleEvent.h"


//TODO DEBUG only
#include "cinder/app/App.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

const string PATH_EXTENSION = ".particle.json";

ParticleEvent::ParticleEvent():
    mRate(1.0f),
    mEmitScale(1.0f),
    mDiffuseColor(Color( 1.0f, 1.0f, 1.0f )),
    mTotalVertices(0),
    mPrevTotalVertices(0),
    mBlendMode(BLEND_OPAQUE),
    mParticleLifetime(Vec2f(0.0f,0.0f)),
    mPreviousElapsed(0.0f),
    mCurrentRate(0.0f)
{ 
    mFileExtension = PATH_EXTENSION;
    registerAttributes(); 
}
    
ParticleEvent::~ParticleEvent()
{
    if (mVerts != NULL)	{
        delete[] mVerts;
        mVerts = NULL;
    }
    
    //mParticles.clear();
}

void ParticleEvent::processAttributes()
{
    // if can associate these member vars with the attr name that would be sweet
    mDiffuseTexture =mAttributes.at("DiffuseTexture").getTexture();
    mDiffuseColor = mAttributes.at("DiffuseColor").getColor();
    mParticleScaleCurve = mAttributes.at("ParticleScale").getCurve();
    mEmitScale = mAttributes.at("EmitScale").getFloat();
    mRate = mAttributes.at("Rate").getFloat();
    mInitialSpeed = mAttributes.at("InitialSpeed").getFloat();
    mLifetime = mAttributes.at("Lifetime").getFloat();
    mParticleLifetime = mAttributes.at("ParticleLifetime").getVector2();
    mEmitMode = EMIT_MODES.at(mAttributes.at("EmitMode").getString());
    mAlphaCurve = mAttributes.at("Alpha").getCurve();
    mBlendMode = BLEND_MODES.at(mAttributes.at("BlendMode").getString());
    
    //mRotationAngle = mAttributes.at("RotationAngle").getFloat();
}

void ParticleEvent::addNewParticle()
{
    Particle newParticle;
    newParticle.rotation = 0.0f;
    newParticle.lifetime = 0.0f;
    newParticle.startTime = -1.0f;
    newParticle.maxLifetime = mParticleLifetime[0] + Rand::randFloat(-mParticleLifetime[1], mParticleLifetime[1]) ;
    newParticle.position = mEmitterPosition + Rand::randVec3f() * mEmitScale;
    newParticle.color = mDiffuseColor;
    mParticles.push_back(newParticle);
}

void ParticleEvent::setup()
{
    processAttributes();
    
    mVerts = NULL;
    mPrevTotalVertices = -1;
    
    mParticles.clear();
    mPreviousElapsed = 0.0f;
    mCurrentRate = 0.0f;
    
    if (mEmitMode == EMIT_BURST)
    {
        for (int i=0; i<mRate; i++)
        {
            addNewParticle();
        }
    }
}

void ParticleEvent::update(const ci::CameraPersp &camera)
{
    float elapsed = getElapsedSeconds();
    
    // run through the state gauntlet
    if (isStarted())
    {
        mEventState = EVENT_RUNNING;
    }
    
    if (isRunning() && getEventElapsedSeconds() >= mLifetime)
    {
        stop(mHardStop);
    }
    
    if (isStopping() && mParticles.size() == 0)
    {
        mEventState = EVENT_STOPPED;
    }
    
    if (isInitialized() || isStopped())
    {
        return;
    }
    
    if (isRunning() && mEmitMode == EMIT_CONTINUOUS)
    {
        float currentElapsed = elapsed - mPreviousElapsed;
        float actualRate = mCurrentRate + mRate * currentElapsed;
        float rateRounded = floor(actualRate);
        
        // carry over any remainder "rate" to the next frame
        mCurrentRate = actualRate - rateRounded;
        
        for (int i=0; i<rateRounded; i++)
        {
            addNewParticle();
        }
        
        mPreviousElapsed = elapsed;
    }
    
    Vec3f bbRight, bbUp;
    camera.getBillboardVectors( &bbRight, &bbUp );   
    
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it )
    {
		if ((*it).startTime == -1.0f)
        {
            (*it).startTime = elapsed;
            (*it).lifetime = 0.0f;
        }
        else
        {
            (*it).lifetime = elapsed - (*it).startTime;
        }
        
        if ((*it).lifetime > (*it).maxLifetime)
        {
            it = mParticles.erase( it );
        }
        
        float interval = (*it).lifetime / (*it).maxLifetime;
        float time = interval - floor(interval);
        
        //TODO here need to grab the value from the curve using time
        (*it).alpha = mAlphaCurve.getPosition(time)[1];
        (*it).scale = mParticleScaleCurve.getPosition(time)[1];
    }
    
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
	
	for( list<Particle>::const_iterator it = mParticles.begin(); it != mParticles.end(); ++it )
    {
        Vec3f pos = (*it).position;
        Color c = (*it).color;
        float scale = (*it).scale;
		Vec4f col = Vec4f(c.r, c.g, c.b, (*it).alpha);
    
        Vec3f right			= bbRight * scale;
        Vec3f up			= bbUp * scale;
        
        Vec3f p1			= pos - right + up;
        Vec3f p2			= pos + right + up;
        Vec3f p3			= pos - right - up;
        Vec3f p4			= pos + right - up;
        
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

void ParticleEvent::enableBlendMode()
{
    switch (mBlendMode) 
    {
        case BLEND_ADDITIVE:
        {
            gl::enableAdditiveBlending();
            break;
        }
        case BLEND_ALPHA:
        {
            gl::enableAlphaBlending();
            break;
        }
        default:
        {
            //opaque!
            return;                
        }
    }
}

void ParticleEvent::disableBlendMode()
{
    switch (mBlendMode) 
    {
        case BLEND_ALPHA:
        {
            gl::disableAlphaBlending();
            break;
        }
        default:
        {
            //opaque or additive
            return;                
        }
    }
}

void ParticleEvent::draw()
{    
    if (isInitialized() || isStopped())
        return;
        
    enableBlendMode();	
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
    disableBlendMode();

    // and then any children will be draw after this
}