#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
//#include "json/json.h"

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
		
	gl::Texture mTex;
	CameraPersp	mCam;
    
    Json::Value mData;
    
    Effect* mTestEffect;
    
    // TODO:  This will be the effect manager eventually, which inherits from BloomSceneRef
    BloomSceneRef mBloomSceneRef;
    CameraPersp mCamera;
    
  protected:
    //void LoadFile( DataSourceRef dataSource 
};

void ParticleTestApp::setup()
{
	mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.001f, 2000.0f );
    mCamera.setWorldUp( Vec3f(0.0f, 1.0f, 0.0f) );
	mCamera.lookAt( Vec3f(0.0f, 0.0f, -50.f), Vec3f::zero());  //TODO rough
    
    //mBloomSceneRef = BloomScene::create( this );
    
    mTestEffect = new Effect("test.effect.json");
    mTestEffect->setCamera(mCamera);
    mTestEffect->setup();
    
    mTestEffect->start();
}

void ParticleTestApp::resize( ResizeEvent event )
{
	//mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	//mCam.setPerspective( 60, event.getAspectRatio(), 1, 1000 );
}

void ParticleTestApp::update()
{
    if (mTestEffect)
    { 
        if (!mTestEffect->isStopped())
        {
            mTestEffect->deepUpdate();
        }
        else
        {
            delete mTestEffect;
            mTestEffect = NULL;
        }
    }

}

void ParticleTestApp::draw()
{
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    //gl::setMatricesWindow( getWindowSize() );
    gl::setMatrices( mCamera );
        
    if (mTestEffect && !mTestEffect->isStopped())
        mTestEffect->draw();
        
}

CINDER_APP_COCOA_TOUCH( ParticleTestApp, RendererGl )
