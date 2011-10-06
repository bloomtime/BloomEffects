#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "ciJson.h"

#include "ParticleEvent.h"
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
    ParticleEvent* mTestEvent;
    
  protected:
  
    //void LoadFile( DataSourceRef dataSource 
};

void ParticleTestApp::setup()
{
    //mData = loadResource("test1.json");
    DataSourceRef dataSource = loadResource("test1.json");
    
    Buffer buf = dataSource->getBuffer();
	size_t dataSize = buf.getDataSize();
	shared_ptr<char> bufString( new char[dataSize+1], checked_array_deleter<char>() );
	memcpy( bufString.get(), buf.getData(), buf.getDataSize() );
	bufString.get()[dataSize] = 0;
    
    string fileString;
    fileString.append(bufString.get());

    mData = json::deserialize(fileString);

    if (mData != NULL) {
        float scale = mData["Scale"].asFloat();
        
        console() << "yay: " << mData["Scale"].asFloat() << " - boo"<< std::endl;
        
        mTestEvent = new ParticleEvent(scale, Vec3f(getWindowCenter(), 0.0f));
    }
    else {
        console() << "ERROR: Invalid Data."<< std::endl;
    }
}

void ParticleTestApp::resize( ResizeEvent event )
{
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCam.setPerspective( 60, event.getAspectRatio(), 1, 1000 );
}

void ParticleTestApp::update()
{
    if (mTestEvent)
        mTestEvent->update();
}

void ParticleTestApp::draw()
{
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    gl::setMatricesWindow( getWindowSize() );

    
    if (mTestEvent)
        mTestEvent->draw();
}

CINDER_APP_COCOA_TOUCH( ParticleTestApp, RendererGl )
