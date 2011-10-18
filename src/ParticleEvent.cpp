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
    mDiffuseColor(Color(1.0f, 1.0f, 1.0f)),
    mTotalVertices(0),
    mPrevTotalVertices(0),
    mBlendMode(BLEND_OPAQUE),
    mParticleLifetime(Vec2f(0.0f, 0.0f)),
    mInitialSpeed(Vec2f(0.0f, 0.0f)),
    mInitialRotation(Vec2f(0.0f, 0.0f)),
    mPreviousElapsed(0.0f),
    mCurrentRate(0.0f),
    mGlobalForce(Vec3f(0.0f, 0.0f, 0.0f)),
    mInheritTransform(false)
{ 
    mEmissionVolume = EmissionVolume();
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
    mRate = mAttributes.at("Rate").getFloat();
    mLifetime = mAttributes.at("Lifetime").getFloat();
    mParticleLifetime = mAttributes.at("ParticleLifetime").getVector2();
    mEmitMode = EMIT_MODES.at(mAttributes.at("EmitMode").getString());
    
    mEmissionVolume.setScale(mAttributes.at("EmitScale").getVector3());
    mEmissionVolume.setVolumeType(EMIT_VOLUMES.at(mAttributes.at("EmitVolumeType").getString()));
    mEmitAngle = mAttributes.at("EmitAngle").getVector2();  
      
    mAlphaCurve = mAttributes.at("Alpha").getCurve();
    mParticleScaleCurve = mAttributes.at("ParticleScale").getCurve();
    
    mDiffuseTexture = mAttributes.at("DiffuseTexture").getTexture();
    mDiffuseColor = mAttributes.at("DiffuseColor").getColor();
    mBlendMode = BLEND_MODES.at(mAttributes.at("BlendMode").getString());
    
    mInitialSpeed = mAttributes.at("InitialSpeed").getVector2();
    mInitialRotation = mAttributes.at("InitialRotation").getVector2();
    mGlobalForce = mAttributes.at("GlobalForce").getVector3();
    mDragForce = mAttributes.at("DragForce").getVector3();
    mRotationSpeed = mAttributes.at("RotationSpeed").getVector2();
    mInheritTransform = mAttributes.at("InheritTransform").getBool();
}

Vec3f ParticleEvent::getEmitDirection()
{
    //Vec2f xz = Rand::randVec2f() * Rand::randFloat(mScale[1], mScale[0]);
    
    float coneSizeX = mEmitAngle[0];
    float coneSizeY = mEmitAngle[1];
    
    float rangeX = toRadians(Rand::randFloat(coneSizeX, coneSizeY));
    float rangeY = toRadians(Rand::randFloat(coneSizeX, coneSizeY));
    Vec2f coneRange = Rand::randVec2f();
    
    Quatf q = Quatf(coneRange[0] * rangeX, coneRange[1] * rangeY, 0.0f);
    Vec3f emitDir = Vec3f(0.0f, 0.0f, 1.0f) * q;
    return emitDir * mEmitterOrientation; 
}

void ParticleEvent::addNewParticle()
{
    Particle newParticle;
    newParticle.rotation = (mInitialRotation[0] + Rand::randFloat(-mInitialRotation[1], mInitialRotation[1]));
    newParticle.rotationSpeed = (mRotationSpeed[0] + Rand::randFloat(-mRotationSpeed[1], mRotationSpeed[1]));
    newParticle.lifetime = 0.0f;
    newParticle.startTime = -1.0f;
    newParticle.maxLifetime = mParticleLifetime[0] + Rand::randFloat(-mParticleLifetime[1], mParticleLifetime[1]);
    
    // if inheriting parent's transform, don't store the source position now--add it later
    if (mInheritTransform)
        newParticle.position = mEmissionVolume.getRandomPoint() * mEmitterOrientation;
    else
        newParticle.position = mEmitterPosition + (mEmissionVolume.getRandomPoint() * mEmitterOrientation);
        
    newParticle.color = mDiffuseColor;
    newParticle.velocity = getEmitDirection() * (mInitialSpeed[0] + Rand::randFloat(-mInitialSpeed[1], mInitialSpeed[1]));
    mParticles.push_back(newParticle);;
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

void ParticleEvent::updateVelocity(Particle &currentParticle, float dt)
{
    Vec3f currentVel = currentParticle.velocity;
    Vec3f drag = Vec3f(0.0f, 0.0f, 0.0f);
    
    if (mDragForce[0] != 0.0f || mDragForce[1] != 0.0f || mDragForce[2] != 0.0f)
    {
        float velSquared = currentVel[0] * currentVel[0] + currentVel[1] * currentVel[1] + currentVel[2] * currentVel[2];
        float velLength = sqrt(velSquared);
        drag = -mDragForce * velSquared / velLength;
    }
    
    currentParticle.velocity += currentVel * drag + mGlobalForce * dt;
}

void ParticleEvent::update(const ci::CameraPersp &camera)
{
    updateEmitter();
    
    float elapsed = getElapsedSeconds();
    float dt = elapsed - mPreviousElapsed;    
    
    // run through the state gauntlet
    if (isStarted())
    {
        mPreviousElapsed = elapsed;
        mEventState = EVENT_RUNNING;
    }
    
    // -1.0f lifetime is infinite lifetime
    if (isRunning() && getEventElapsedSeconds() >= mLifetime && mLifetime != -1.0f)
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
    }
    
    Vec3f bbRight, bbUp, bbAt;
    camera.getBillboardVectors( &bbRight, &bbUp );  
    bbAt = cross(bbUp, bbRight);
    
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
        
        (*it).alpha = mAlphaCurve.getPosition(time)[1];
        (*it).scale = mParticleScaleCurve.getPosition(time)[1];
        
        // update particle position after applying forces
        updateVelocity(*it, dt);
        
        (*it).position = (*it).position + (*it).velocity;    

        // update rotation
        float rot = (*it).rotation + (*it).rotationSpeed * dt;
        
        if (rot >= 360.0f)
            rot = rot - 360.0f;
        else if (rot < 0.0f)
            rot = rot + 360;
            
        (*it).rotation = rot;
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
        
        // if inheriting parent's transform, add it in now since it might have updated
        if (mInheritTransform)
            pos += mEmitterPosition;
        
        float rot = toRadians((*it).rotation);
        Color c = (*it).color;
        float scale = (*it).scale;
		Vec4f col = Vec4f(c.r, c.g, c.b, (*it).alpha);
    
        Vec3f right = Quatf( bbAt, rot ) * bbRight * scale;
        Vec3f up = Quatf( bbAt, rot ) * bbUp * scale;
        
        Vec3f p1 = pos - right + up;
        Vec3f p2 = pos + right + up;
        Vec3f p3 = pos - right - up;
        Vec3f p4 = pos + right - up;
        
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
    
    mPreviousElapsed = elapsed;
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