//
//  ParticleEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "ParticleEvent.h"

#include "cinder/Rand.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

using namespace ci;

const string PATH_EXTENSION = ".particle.json";

ParticleEventRef ParticleEvent::create()
{
    return ParticleEventRef( new ParticleEvent() );
}

ParticleEvent::ParticleEvent():
    mRate(1.0f),
    mTotalVertices(0),
    mPrevTotalVertices(0),
    mBlendMode(BLEND_OPAQUE),
    mParticleLifetime(Vec2f(1.0f, 0.0f)),
    mInitialSpeed(Vec2f(0.0f, 0.0f)),
    mInitialRotation(Vec2f(0.0f, 0.0f)),
    mPreviousElapsed(0.0f),
    mCurrentRate(0.0f),
    mGlobalForce(Vec3f(0.0f, 0.0f, 0.0f)),
    mTiledTexture(true),
    mBlendTiles(false),
    mEmitMode(EMIT_BURST),
    mEmitAngle(Vec2f(0.0f, 0.0f)),
    mNumTiles(1.0f),
    mTileWidth(1.0f),
    mVerts(NULL),
    mRotationAngle(0.0f),
    mRotationSpeed(Vec2f(0.0f, 0.0f)),
    mCameraAttached(false),
    mDragForce(Vec3f(0.0f, 0.0f, 0.0f)),
    mScreenSizeLOD(Vec2f(0.0f, 2.0f)),
    mTintColorAlpha(Vec4f(1.0f, 1.0f, 1.0f, 1.0f)),
    mTileUVLerp(Vec3f(0.0f, 0.0f, 0.0f)),
    mDiffuseRedLerp(Vec3f(0.0f, 0.0f, 0.0f)),
    mDiffuseGreenLerp(Vec3f(0.0f, 0.0f, 0.0f)),
    mDiffuseBlueLerp(Vec3f(0.0f, 0.0f, 0.0f)),
    mWindowWidth(1.0f),
    mWindowHeight(1.0f)
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
    
    mParticles.clear();
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
      
    mAlphaCurve = mAttributes.at("Alpha").getCurvePoints();
    mParticleScaleCurve = mAttributes.at("ParticleScale").getCurvePoints();
    
    mDiffuseRedLerp = mAttributes.at("DiffuseColorR").getVector3();
    mDiffuseGreenLerp = mAttributes.at("DiffuseColorG").getVector3();
    mDiffuseBlueLerp = mAttributes.at("DiffuseColorB").getVector3();
    mTileUVLerp = mAttributes.at("TileUV").getVector3();
    mBlendMode = BLEND_MODES.at(mAttributes.at("BlendMode").getString());
    mTiledTexture = mAttributes.at("TiledTexture").getBool();
    mBlendTiles = mAttributes.at("BlendTiles").getBool();
    
    mDiffuseTexture = mAttributes.at("DiffuseTexture").getTexture();
    mDiffuseTexture.setWrap(GL_REPEAT, GL_REPEAT);
    
    mShader = mAttributes.at("Shader").getShader();
    pos_handle = mShader.getAttribLocation("a_position");
    txc_handle = mShader.getAttribLocation("a_texcoord");
    col_handle = mShader.getAttribLocation("a_color");
    tile_handle = mShader.getAttribLocation("a_tileIndex");
    
    mInitialSpeed = mAttributes.at("InitialSpeed").getVector2();
    mInitialRotation = mAttributes.at("InitialRotation").getVector2();
    mGlobalForce = mAttributes.at("GlobalForce").getVector3();
    mDragForce = mAttributes.at("DragForce").getVector3();
    mRotationSpeed = mAttributes.at("RotationSpeed").getVector2();
    mInheritTransform = mAttributes.at("InheritTransform").getBool();
    mCameraAttached = mAttributes.at("CameraAttached").getBool();
    mScreenSizeLOD = mAttributes.at("ScreenSizeLOD").getVector2();
}

Vec3f ParticleEvent::getEmitDirection()
{
    float coneSizeX = mEmitAngle[0];
    float coneSizeY = mEmitAngle[1];
    
    float rangeX = toRadians(Rand::randFloat(coneSizeX, coneSizeY));
    float rangeY = toRadians(Rand::randFloat(coneSizeX, coneSizeY));
    Vec2f coneRange = Rand::randVec2f();
    
    Quatf q = Quatf(coneRange[0] * rangeX, coneRange[1] * rangeY, 0.0f);
    Vec3f emitDir = Vec3f(0.0f, 0.0f, 1.0f) * q;
    return emitDir * mSourceOrientation; 
}

floatCurve ParticleEvent::getNewCurve(AttributeCurvePoints &curvePoints)
{
    vector<Vec2f> curveInput;
 
    vector<Vec2f> valuePoints = curvePoints.valuePoints;
    vector<Vec2f> variancePoints = curvePoints.variancePoints;
       
    Vec2f previousPoint = Vec2f(valuePoints[0][0], valuePoints[0][1] + Rand::randFloat(-variancePoints[0][1], variancePoints[0][1]));
    Vec2f currentPoint = Vec2f(valuePoints[1][0], valuePoints[1][1] + Rand::randFloat(-variancePoints[1][1], variancePoints[1][1]));
    
    // two tangent points (b1 and b2) per control point
    for (int i=1; i < valuePoints.size()-1; i++)
    {            
        Vec2f nextPoint = Vec2f(valuePoints[i+1][0], valuePoints[i+1][1] + Rand::randFloat(-variancePoints[i+1][1], variancePoints[i+1][1]));

        Vec2f b1 = currentPoint - (currentPoint - previousPoint) * TANGENT_LENGTH;
        Vec2f b2 = currentPoint + (nextPoint - currentPoint) * TANGENT_LENGTH;
        
        curveInput.push_back(b1);
        curveInput.push_back(currentPoint);
        curveInput.push_back(b2);
        
        currentPoint = nextPoint;
        previousPoint = currentPoint;
    }

    return BSpline2f( curveInput, 3, false, true );
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
        newParticle.position = mEmissionVolume.getRandomPoint(mSourceScale) * mSourceOrientation;
    else
        newParticle.position = mSourcePosition + (mEmissionVolume.getRandomPoint(mSourceScale) * mSourceOrientation);
        
    newParticle.velocity = getEmitDirection() * (mInitialSpeed[0] + Rand::randFloat(-mInitialSpeed[1], mInitialSpeed[1]));
    
    // store curve values that have variance baked in
    newParticle.alphaCurve = getNewCurve(mAlphaCurve);
    newParticle.scaleCurve = getNewCurve(mParticleScaleCurve);
    
    // store lerp values with variance baked in
    float tileUVVariance = Rand::randFloat(-mTileUVLerp.z, mTileUVLerp.z);
    newParticle.tileUVLerp = Vec2f(mTileUVLerp.x + tileUVVariance, mTileUVLerp.y + tileUVVariance);
    
    float redVariance = Rand::randFloat(-mDiffuseRedLerp.z, mDiffuseRedLerp.z);
    newParticle.diffuseRedLerp = Vec2f(math<float>::clamp(mDiffuseRedLerp.x + redVariance, 0.0f, 1.0f), 
                                       math<float>::clamp(mDiffuseRedLerp.y + redVariance, 0.0f, 1.0f));
     
    float greenVariance = Rand::randFloat(-mDiffuseGreenLerp.z, mDiffuseGreenLerp.z);
    newParticle.diffuseGreenLerp = Vec2f(math<float>::clamp(mDiffuseGreenLerp.x + greenVariance, 0.0f, 1.0f), 
                                         math<float>::clamp(mDiffuseGreenLerp.y + greenVariance, 0.0f, 1.0f));
                      
    float blueVariance = Rand::randFloat(-mDiffuseBlueLerp.z, mDiffuseBlueLerp.z);
    newParticle.diffuseBlueLerp = Vec2f(math<float>::clamp(mDiffuseBlueLerp.x + blueVariance, 0.0f, 1.0f), 
                                        math<float>::clamp(mDiffuseBlueLerp.y + blueVariance, 0.0f, 1.0f));
    
    mParticles.push_back(newParticle);
}

void ParticleEvent::setup()
{
    //TODO may have to listen for size changes later
    mWindowWidth = ci::app::getWindowWidth();
    mWindowHeight = ci::app::getWindowHeight();
    
    processAttributes();

    if (mCameraAttached)
    {
        Matrix44f mtx = mCamera->getOrientation().toMatrix44();
        Vec3f trans = mCamera->getEyePoint();
        mtx.setTranslate(trans);
            
        setParentTransform(mtx);
    }
    
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
    
    if (mTiledTexture)
    {
        mNumTiles = (float)mDiffuseTexture.getWidth()/(float)mDiffuseTexture.getHeight();
        mTileWidth = (float)mDiffuseTexture.getHeight()/(float)mDiffuseTexture.getWidth();
    }
    
    //TODO VBO
    //glGenBuffers(1, &vtx_buffer); 
    //glBindBuffer(GL_ARRAY_BUFFER, vtx_buffer);
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

Vec2f ParticleEvent::getNormalizedScreenPos(Vec3f worldPos)
{
    Vec2f screenPos =  mCamera->worldToScreen(worldPos, mWindowWidth, mWindowHeight);
    return Vec2f(screenPos.x / mWindowWidth, screenPos.y / mWindowHeight);
}

void ParticleEvent::update()
{
    //Vec3f eyePoint = mCamera->getEyePoint();
    
    if (mCameraAttached)
    {
        Matrix44f mtx = mCamera->getOrientation().toMatrix44();
        Vec3f trans = mCamera->getEyePoint();
        mtx.setTranslate(trans);
            
        setParentTransform(mtx);
    }
    
    updateSource();
    
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
    mCamera->getBillboardVectors( &bbRight, &bbUp );  
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
            continue;
        }
        
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
        }
        
        mPrevTotalVertices = mTotalVertices;
    }
        
	int vIndex = 0;
	
	for( list<Particle>::const_iterator it = mParticles.begin(); it != mParticles.end(); ++it )
    {
        Vec3f pos = (*it).position;
        
        // if inheriting parent's transform, add it in now since it might have updated
        if (mInheritTransform)
            pos += mSourcePosition;
        
        float rot = toRadians((*it).rotation);
        
        float interval = (*it).lifetime / (*it).maxLifetime;
        float time = interval - floor(interval);
        
        // curve lookups
        float alpha = (*it).alphaCurve.getPosition(time)[1];
        float scale = (*it).scaleCurve.getPosition(time)[1] * mSourceScale;

        // instead of curves, these just lerp
        float tileIndex = lerp((*it).tileUVLerp.x, (*it).tileUVLerp.y, time) * mNumTiles;
        float colorR = lerp((*it).diffuseRedLerp.x, (*it).diffuseRedLerp.y, time);
        float colorG = lerp((*it).diffuseGreenLerp.x, (*it).diffuseGreenLerp.y, time);
        float colorB = lerp((*it).diffuseBlueLerp.x, (*it).diffuseBlueLerp.y, time);
        
		Vec4f col = Vec4f(colorR, colorG, colorB, alpha);
        
        Vec3f right = Quatf( bbAt, rot ) * bbRight * scale;
        Vec3f up = Quatf( bbAt, rot ) * bbUp * scale;
        
        Vec3f p1 = pos - right + up;
        Vec3f p2 = pos + right + up;
        Vec3f p3 = pos - right - up;
        Vec3f p4 = pos + right - up;
        
        //TODO here camera culling and LOD'ing (screen space size)
        // here, need own camera calculation stuffs
        Vec2f screenPosA = getNormalizedScreenPos(p1);
        Vec2f screenPosB = getNormalizedScreenPos(p4);
        
        float screenLength = (screenPosB - screenPosA).length();
        
        // hardcoded fading threshold for now--user probably doesn't care that much.
        float fadeRange = .05f;
        float minFade = mScreenSizeLOD.x + fadeRange;
        float maxFade = mScreenSizeLOD.y - fadeRange;
        
        // test only two corners for now to see if that is enough
        if ((screenPosA.x < 0.0f || screenPosA.x > 1.0f) && 
            (screenPosA.x < 0.0f || screenPosA.x > 1.0f) &&
            (screenPosB.x < 0.0f || screenPosB.x > 1.0f) && 
            (screenPosB.x < 0.0f || screenPosB.x > 1.0f))
        {
            alpha = 0.0f;
        }
        else if (screenLength <= minFade)
        {
            float fadeAmt = (screenLength - mScreenSizeLOD.x) / (minFade - mScreenSizeLOD.x);
            alpha *= fadeAmt;
        }
        else if (screenLength >= maxFade)
        {
            float fadeAmt = (screenLength - maxFade) / (mScreenSizeLOD.y - maxFade);
            alpha *= fadeAmt;
        }
        
        // don't draw if alpha 0
        if (alpha == 0)
        {
            mTotalVertices-= 6;
            //realloc(mVerts, mTotalVertices * sizeof(VertexData));
            continue;
        }

        mVerts[vIndex].vertex  = p1;
        mVerts[vIndex].texture = Vec2f(0.0f,0.0f);
        mVerts[vIndex].color   = col;
        mVerts[vIndex].tileIndex = tileIndex;
        vIndex++;
        
        mVerts[vIndex].vertex  = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color   = col;
        mVerts[vIndex].tileIndex = tileIndex;
        vIndex++;
        
        mVerts[vIndex].vertex  = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color   = col;
        mVerts[vIndex].tileIndex = tileIndex;
        vIndex++;
        
        mVerts[vIndex].vertex  = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color   = col;
        mVerts[vIndex].tileIndex = tileIndex;
        vIndex++;
        
        mVerts[vIndex].vertex  = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color   = col;
        mVerts[vIndex].tileIndex = tileIndex;
        vIndex++;
        
        mVerts[vIndex].vertex  = p4;
        mVerts[vIndex].texture = Vec2f(1.0f,1.0f);
        mVerts[vIndex].color   = col;
        mVerts[vIndex].tileIndex = tileIndex;
        vIndex++;        
        
	}    
    
    mPreviousElapsed = elapsed;
    
    //TODO VBO
    //glBufferData(GL_ARRAY_BUFFER, mTotalVertices * sizeof(mVerts[0]), &mVerts[0], GL_DYNAMIC_DRAW);
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
    if (isInitialized() || isStopped() || mTotalVertices == 0)
        return;
    
    enableBlendMode();	

    mShader.bind();
    mDiffuseTexture.bind(0);
    //glBindBuffer(GL_ARRAY_BUFFER, vtx_buffer);
    
    // not needed since there is only one texture at the moment
    // mShader.uniform("u_diffuseTex", 1);
    
    mShader.uniform("u_tileWidth", mTileWidth);
    mShader.uniform("u_tileBlend", mBlendTiles);
    mShader.uniform("u_tintColor", mTintColorAlpha);
    mShader.uniform("u_mvp_matrix", mCamera->getProjectionMatrix() * mCamera->getModelViewMatrix());
    
    GLsizei stride = sizeof(VertexData);
    //const GLvoid* colOffset = (GLvoid*) sizeof(vec3);
    //const GLvoid* texOffset = (GLvoid*) sizeof(vec3);
    //const GLvoid* tileOffset = (GLvoid*) sizeof(vec3);
    
    glVertexAttribPointer(pos_handle, 3, GL_FLOAT, GL_FALSE, stride, &mVerts[0].vertex);    
    glVertexAttribPointer(col_handle, 4, GL_FLOAT, GL_FALSE, stride, &mVerts[0].color);
    glVertexAttribPointer(txc_handle, 2, GL_FLOAT, GL_FALSE, stride, &mVerts[0].texture);
    glVertexAttribPointer(tile_handle, 1, GL_FLOAT, GL_FALSE, stride, &mVerts[0].tileIndex);

    glEnableVertexAttribArray(pos_handle);
    glEnableVertexAttribArray(txc_handle);
    glEnableVertexAttribArray(col_handle);
    glEnableVertexAttribArray(tile_handle);
    
    glDrawArrays(GL_TRIANGLES, 0, mTotalVertices);
    
    glDisableVertexAttribArray(tile_handle);
    glDisableVertexAttribArray(col_handle);
    glDisableVertexAttribArray(txc_handle);
    glDisableVertexAttribArray(pos_handle);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

 	mDiffuseTexture.unbind();
    mShader.unbind();
       
    disableBlendMode();
}