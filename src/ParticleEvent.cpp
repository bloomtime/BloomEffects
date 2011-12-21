//
//  ParticleEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "ParticleEvent.h"

#include "cinder/Rand.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;

const string PATH_EXTENSION = ".particle.json";

ParticleEventRef ParticleEvent::create()
{
    return ParticleEventRef( new ParticleEvent() );
}

ParticleEvent::ParticleEvent():
    mRate(1.0f),
    mTotalVertices(0),
    mBlendMode(BLEND_OPAQUE),
    mParticleLifetime(Vec2f(1.0f, 0.0f)),
    mInitialSpeed(Vec2f(0.0f, 0.0f)),
    mInitialRotation(Vec2f(0.0f, 0.0f)),
    mPreviousElapsed(0.0f),
    mCurrentRate(0.0f),
    mGlobalForce(Vec3f(0.0f, 0.0f, 0.0f)),
    mTiledTexture(true),
    mBlendTiles(false),
    mTexture2Mode(TEX2_NONE),
    mEmitMode(EMIT_BURST),
    mEmitAngle(Vec2f(0.0f, 0.0f)),
    mNumTiles(1.0f),
    mTileWidth(1.0f),
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
    mWindowHeight(1.0f),
    mTimeElapsed(0.0f)
{ 
    mLifetime = 1.0f;
    mEmissionVolume = EmissionVolume();
    mFileExtension = PATH_EXTENSION;
    registerAttributes(); 
}
    
ParticleEvent::~ParticleEvent()
{
    mParticles.clear();
}

void ParticleEvent::processAttributes()
{
    // if can associate these member vars with the attr name that would be sweet
    mRate = mAttributes.at("Rate").getFloat();
    mLifetime = mAttributes.at("Lifetime").getFloat();
    mParticleLifetime = mAttributes.at("ParticleLifetime").getVector2();
    mEmitMode = EMIT_MODES.at(mAttributes.at("EmitMode").getString());
    mFaceMode = FACE_MODES.at(mAttributes.at("FacingMode").getString());
    
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
    mTexture2Mode = TEXTURE2_MODES.at(mAttributes.at("Texture2Mode").getString());
    
    mDiffuseTexture = mAttributes.at("DiffuseTexture").getTexture();
    mDiffuseTexture.setWrap(GL_REPEAT, GL_REPEAT);
    mSecondaryTexture = mAttributes.at("SecondaryTexture").getTexture();
    mSecondaryTexture.setWrap(GL_REPEAT, GL_REPEAT);
    
    mShader = mAttributes.at("Shader").getShader();
    
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

void ParticleEvent::setup(Vec2f winSize)
{
    //TODO may have to listen for size changes later
    setWindowDimensions(winSize.x, winSize.y);
    
    processAttributes();

    if (mCameraAttached)
    {
        Matrix44f mtx = mCamera->getOrientation().toMatrix44();
        Vec3f trans = mCamera->getEyePoint();
        mtx.setTranslate(trans);
            
        setParentTransform(mtx);
    }
    
    if (mTiledTexture)
    {
        mNumTiles = (float)mDiffuseTexture.getWidth()/(float)mDiffuseTexture.getHeight();
        mTileWidth = (float)mDiffuseTexture.getHeight()/(float)mDiffuseTexture.getWidth();
    }
    
    mTrianglesVBO = gl::Vbo::create(GL_TRIANGLES);
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_position", 3, GL_FLOAT, GL_STREAM_DRAW));
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_texcoord", 2, GL_FLOAT, GL_STREAM_DRAW));
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_color", 4, GL_FLOAT, GL_STREAM_DRAW));
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_normal", 3, GL_FLOAT, GL_STREAM_DRAW));
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_tangent", 3, GL_FLOAT, GL_STREAM_DRAW));
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_bitangent", 3, GL_FLOAT, GL_STREAM_DRAW));
    mTrianglesVBO->set(gl::Vbo::Attribute::create("a_tileIndex", 1, GL_FLOAT, GL_STREAM_DRAW));
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
    if (mCameraAttached)
    {
        Matrix44f mtx = mCamera->getOrientation().toMatrix44();
        Vec3f trans = mCamera->getEyePoint();
        mtx.setTranslate(trans);
            
        setParentTransform(mtx);
    }
    
    updateSource();
    
    float totalElapsed = getEventElapsedSeconds();
    float dt = totalElapsed - mPreviousElapsed;    
    mTimeElapsed = totalElapsed * 1000.0f;
    
    mPreviousElapsed = totalElapsed;
    
    // run through the state gauntlet
    if (isStarted())
    {
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
        
        mEventState = EVENT_RUNNING;
    }
    
    // -1.0f lifetime is infinite lifetime
    if (isRunning() && totalElapsed >= mLifetime && mLifetime != -1.0f)
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
        float actualRate = mCurrentRate + mRate * dt;
        float rateRounded = floor(actualRate);
        
        // carry over any remainder "rate" to the next frame
        mCurrentRate = actualRate - rateRounded;
        
        for (int i=0; i<rateRounded; i++)
        {
            addNewParticle();
        }
    }
    
    Vec3f bbRight, bbUp, bbAt;
    
    if (mFaceMode == FACE_AXIS_X)
    {
        bbUp = Vec3f(0.0f, 1.0f, 0.0f);
        bbRight = Vec3f(0.0f, 0.0f, 1.0f);
    }
    else if (mFaceMode == FACE_AXIS_Y)
    {
        bbUp = Vec3f(-1.0f, 0.0f, 0.0f);
        bbRight = Vec3f(0.0f, 0.0f, -1.0f);
    }
    else if (mFaceMode == FACE_AXIS_Z)
    {
        bbUp = Vec3f(0.0f, 1.0f, 0.0f);
        bbRight = Vec3f(1.0f, 0.0f, 0.0f);
    }
    else // FACE_BILLBOARD
    {
        mCamera->getBillboardVectors( &bbRight, &bbUp ); 
    }
    
    bbAt = cross(bbUp, bbRight);
    
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it )
    {
		if ((*it).startTime == -1.0f)
        {
            (*it).startTime = totalElapsed;
            (*it).lifetime = 0.0f;
        }
        else
        {
            (*it).lifetime = totalElapsed - (*it).startTime;
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
    
	vector<Vec3f> positions;
    vector<Vec2f> texcoords;
    vector<ColorA> colors;
    vector<Vec3f> normals;
    vector<Vec3f> tangents;
    vector<Vec3f> bitangents;
    vector<float> tileIndices;
    
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
        
		ColorA col = ColorA(colorR, colorG, colorB, alpha);
        
        Vec3f right = Quatf( bbAt, rot ) * bbRight;
        Vec3f up = Quatf( bbAt, rot ) * bbUp;
        
        Vec3f rightScale = right * scale;
        Vec3f upScale = up * scale;
        
        Vec3f p1 = pos - rightScale + upScale;
        Vec3f p2 = pos + rightScale + upScale;
        Vec3f p3 = pos - rightScale - upScale;
        Vec3f p4 = pos + rightScale - upScale;
        
        //camera culling and LOD'ing (screen space size)
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
            float fadeAmt = math<float>::clamp(screenLength - mScreenSizeLOD.x, 0.0, 1.0f) / fadeRange;
            alpha *= fadeAmt;
        }
        else if (screenLength >= maxFade)
        {
            float fadeAmt = (screenLength - maxFade) / fadeRange;
            alpha *= fadeAmt;
        }
        
        // don't draw if alpha 0
        if (alpha == 0)
        {
            mTotalVertices-= 6;
            //realloc(mVerts, mTotalVertices * sizeof(VertexData));
            continue;
        }
        
        Matrix33f rotMatrix = Matrix33f(right, up, bbAt);

        rotMatrix = rotMatrix.inverted().transposed();
        //rotMatrix = rotMatrix.transposed().inverted();
        //Vec3f lightDir = rotMatrix * mKeyLightDir;
        
        Vec3f newRight = rotMatrix.getRow(0);
        Vec3f newUp = rotMatrix.getRow(1);
        Vec3f newAt = rotMatrix.getRow(2);
        
        //TRIANGLE 1, VERTEX 1
        positions.push_back(p1);
        texcoords.push_back(Vec2f(0.0f,0.0f));
        colors.push_back(col);
        normals.push_back(newAt);
        tangents.push_back(newRight);
        bitangents.push_back(newUp);
        tileIndices.push_back(tileIndex);
        
        //TRIANGLE 1, VERTEX 2
        positions.push_back(p2);
        texcoords.push_back(Vec2f(1.0f,0.0f));
        colors.push_back(col);
        normals.push_back(newAt);
        tangents.push_back(newRight);
        bitangents.push_back(newUp);
        tileIndices.push_back(tileIndex);
        
        //TRIANGLE 1, VERTEX 3
        positions.push_back(p3);
        texcoords.push_back(Vec2f(0.0f,1.0f));
        colors.push_back(col);
        normals.push_back(newAt);
        tangents.push_back(newRight);
        bitangents.push_back(newUp);
        tileIndices.push_back(tileIndex);
        
        //TRIANGLE 2, VERTEX 1
        positions.push_back(p2);
        texcoords.push_back(Vec2f(1.0f,0.0f));
        colors.push_back(col);
        normals.push_back(newAt);
        tangents.push_back(newRight);
        bitangents.push_back(newUp);
        tileIndices.push_back(tileIndex);
        
        //TRIANGLE 2, VERTEX 2
        positions.push_back(p3);
        texcoords.push_back(Vec2f(0.0f,1.0f));
        colors.push_back(col);
        normals.push_back(newAt);
        tangents.push_back(newRight);
        bitangents.push_back(newUp);
        tileIndices.push_back(tileIndex);
        
        //TRIANGLE 2, VERTEX 3
        positions.push_back(p4);
        texcoords.push_back(Vec2f(1.0f,1.0f));
        colors.push_back(col);
        normals.push_back(newAt);
        tangents.push_back(newRight);
        bitangents.push_back(newUp);
        tileIndices.push_back(tileIndex);
	}   
    
    mTrianglesVBO->get("a_position")->setData(positions);
    mTrianglesVBO->get("a_texcoord")->setData(texcoords);
    mTrianglesVBO->get("a_color")->setData(colors);
    mTrianglesVBO->get("a_normal")->setData(normals);
    mTrianglesVBO->get("a_tangent")->setData(tangents);
    mTrianglesVBO->get("a_bitangent")->setData(bitangents);
    mTrianglesVBO->get("a_tileIndex")->setData(tileIndices);
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
    mSecondaryTexture.bind(1);
    //glBindBuffer(GL_ARRAY_BUFFER, vtx_buffer);

    mShader.uniform("u_diffuseTex", 0);
    mShader.uniform("u_secondaryTex", 1);
    mShader.uniform("u_tileWidth", mTileWidth);
    mShader.uniform("u_tileBlend", mBlendTiles);
    mShader.uniform("u_hasNormal", mTexture2Mode == TEX2_NORMAL);
    mShader.uniform("u_hasDistort", mTexture2Mode == TEX2_DISTORT);
    mShader.uniform("u_tintColor", mTintColorAlpha);
    mShader.uniform("u_keyLightDir", mKeyLightDir);
    mShader.uniform("u_worldTime", mTimeElapsed);
    mShader.uniform("u_mvp_matrix", mCamera->getProjectionMatrix() * mCamera->getModelViewMatrix());
        
    mTrianglesVBO->draw(mShader);
        
    mSecondaryTexture.unbind();
 	mDiffuseTexture.unbind();
    mShader.unbind();
       
    disableBlendMode();
}