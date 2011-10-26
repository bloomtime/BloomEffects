#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/Camera.h"
#include "cinder/Arcball.h"

#include "EffectsManager.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace ci;
using namespace ci::app;

class ParticleTestApp : public AppCocoaTouch {
  public:
    void prepareSettings(Settings *settings);
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
    
    virtual void    touchesBegan( TouchEvent event );
    virtual void    touchesMoved( TouchEvent event );
    
    EffectsManagerRef mEffectsManager;
    
    CameraPersp mCamera;
    Arcball mArcball;
    Vec3f mUp, mEye, mTarget;
};

void ParticleTestApp::prepareSettings(Settings *settings)
{
    settings->enableMultiTouch(true);
    settings->setFrameRate(60);
}

void ParticleTestApp::setup()
{
    mUp = Vec3f(0.0f, 1.0f, 0.0f);
    mEye = Vec3f(0.0f, 0.0f, -50.f);
    mTarget = Vec3f::zero();
    
    mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.001f, 2000.0f );
	mCamera.lookAt( mEye, mTarget, mUp );  //TODO rough

    mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 500.0f );
    
    mEffectsManager = EffectsManager::create();
    mEffectsManager->setCamera(&mCamera);
    mEffectsManager->setup();
    
    // just a test
    EffectRef testEffect = mEffectsManager->createEffect("test");
    ci::Matrix44f transform;
    transform.setToIdentity();
    
    testEffect->setTransform(transform);
}

void ParticleTestApp::touchesBegan( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = getActiveTouches();
    if (touches.size() == 1) {
        mArcball.mouseDown( touches[0].getPos() );
    }
}
void ParticleTestApp::touchesMoved( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = getActiveTouches();
    if (touches.size() == 1) {
        mArcball.mouseDrag( touches[0].getPos() );
    }    
}

void ParticleTestApp::update()
{
    // Arcball is normally applied to a scene (world)
    // but we're going to apply it to our camera
    // so reverse the angle (but keep the axis)
    Quatf q = mArcball.getQuat();
    q.w *= -1.0;
    
    mCamera.lookAt( q * mEye, mTarget, q * mUp );
    
    mEffectsManager->update();
}

void ParticleTestApp::draw()
{  
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    
    mEffectsManager->draw();
}

CINDER_APP_COCOA_TOUCH( ParticleTestApp, RendererGl )
