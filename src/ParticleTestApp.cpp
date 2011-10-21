#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/gl/Fbo.h"
//#include "json/json.h"

#include "Effect.h"
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
		
	gl::Texture mTex;
    
    Json::Value mData;
    
    Effect* mTestEffect;
    
    CameraPersp mCamera;
    
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
    
  protected:
    //void LoadFile( DataSourceRef dataSource 
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
    
    //mBloomSceneRef = BloomScene::create( this );
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    
    glGenFramebuffers(1, &m_framebuffer); 
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer); 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);

    //glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, getWindowWidth(), getWindowHeight());

    mTestEffect = new Effect("test.effect.json");
    mTestEffect->setCamera(mCamera);
    mTestEffect->setup();
    
    mTestEffect->start();
    
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

	//fgl::enableDepthRead();
	//gl::enableDepthWrite();
    
    
    //Need to put this somewhere maybe in cleanup
    //glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), mVerts, GL_DYNAMIC_DRAW);  //maybe use GL_STATIC_DRAW?
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleTestApp::draw()
{  
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    //gl::setMatrices( mCamera );
        
    if (mTestEffect && !mTestEffect->isStopped())
        mTestEffect->draw();
        
}

CINDER_APP_COCOA_TOUCH( ParticleTestApp, RendererGl )
