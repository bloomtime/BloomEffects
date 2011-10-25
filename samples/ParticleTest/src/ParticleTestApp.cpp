#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/Camera.h"

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
    
    EffectsManagerRef mEffectsManager;
    
    CameraPersp mCamera;
};

void ParticleTestApp::prepareSettings(Settings *settings)
{
    settings->enableMultiTouch(true);
    settings->setFrameRate(60);
}

void ParticleTestApp::setup()
{
    mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.001f, 2000.0f );
    mCamera.setWorldUp( Vec3f(0.0f, 1.0f, 0.0f) );
	mCamera.lookAt( Vec3f(0.0f, 0.0f, -50.f), Vec3f::zero());  //TODO rough
    
    mEffectsManager = EffectsManager::create();
    mEffectsManager->setCamera(&mCamera);
    mEffectsManager->setup();
    
    // just a test
    EffectRef testEffect = mEffectsManager->createEffect("test");
    ci::Matrix44f transform;
    transform.setToIdentity();
    
    testEffect->setTransform(transform);
}

void ParticleTestApp::update()
{
    mEffectsManager->update();
}

void ParticleTestApp::draw()
{  
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    
    mEffectsManager->draw();
}

CINDER_APP_COCOA_TOUCH( ParticleTestApp, RendererGl )
