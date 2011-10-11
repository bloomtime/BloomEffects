#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "ciJson.h"

#include "Effect.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace ci;
using namespace ci::app;

class ParticleTestApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	resize( ResizeEvent event );
	virtual void	update();
	virtual void	draw();
		

	Matrix44f	mCubeRotation;
	gl::Texture mTex;
	CameraPersp	mCam;
    
    json::Value mData;
    
    Effect* mTestEffect;
    //ParticleEvent* mTestEvent;
    
    CameraPersp mCamera;
    
  protected:
  
    //void LoadFile( DataSourceRef dataSource 
};

void ParticleTestApp::setup()
{
	mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.001f, 2000.0f );
	mCamera.lookAt( Vec3f(0.0f, 0.0f, -50.f), Vec3f::zero());  //TODO rough
    
    mTestEffect = new Effect("test.effect.json");
    mTestEffect->setCamera(mCamera);
    mTestEffect->setup();
}

void ParticleTestApp::resize( ResizeEvent event )
{
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCam.setPerspective( 60, event.getAspectRatio(), 1, 1000 );
}

void ParticleTestApp::update()
{
    if (mTestEffect)
        mTestEffect->deepUpdate();
        
    //if (mTestEvent)
    //    mTestEvent->update(mCamera);
}

void ParticleTestApp::draw()
{
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    //gl::setMatricesWindow( getWindowSize() );
    gl::setMatrices( mCamera );
    
    //gl::color( Color::white() );
    //gl::drawSolidCircle( Vec2f::zero(), 3 );
        
    if (mTestEffect)
        mTestEffect->draw();
        
}

CINDER_APP_COCOA_TOUCH( ParticleTestApp, RendererGl )
