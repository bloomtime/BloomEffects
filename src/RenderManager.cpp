//
//  RenderManager.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "RenderManager.h"
#include "cinder/app/App.h"

namespace bloom { namespace effects {

using namespace std;
using namespace ci;

RenderManagerRef RenderManager::create(std::string defaultPost, PreLoaderRef preloader)
{
    return RenderManagerRef( new RenderManager(defaultPost, preloader) );
}

RenderManager::RenderManager(std::string defaultPost, PreLoaderRef preloader)
{
    mBGColor = Color(1.0f, 1.0f, 1.0f);
    mPostShaderAlpha = 1.0f;
    mWindowSize = Vec2i(1024, 768);
    
    setPostShader(defaultPost);
    mDefaultPostShader = mCurrentPostShader;
     
    mVBO = gl::Vbo::create(GL_TRIANGLE_STRIP);
    mVBO->set(gl::Vbo::Attribute::create("a_position", 2, GL_FLOAT, GL_STREAM_DRAW));
    mVBO->set(gl::Vbo::Attribute::create("a_texcoord", 2, GL_FLOAT, GL_STREAM_DRAW));
    
    mPostVBO = gl::Vbo::create(GL_TRIANGLE_STRIP);
    mPostVBO->set(gl::Vbo::Attribute::create("a_position", 2, GL_FLOAT, GL_STREAM_DRAW));
    mPostVBO->set(gl::Vbo::Attribute::create("a_texcoord", 2, GL_FLOAT, GL_STREAM_DRAW));
    
    mPreLoader = preloader;
}

RenderManager::~RenderManager()
{
}

ci::gl::GlslProg RenderManager::getShader(string shaderName)
{
    if (mPreLoader)
    {
        return mPreLoader->getShader(shaderName);
    }
    else 
    {
        return gl::GlslProg(app::App::loadResource(shaderName + "_vert.glsl"), app::App::loadResource(shaderName + "_frag.glsl"));
    }
}

void RenderManager::setPostShader(string shaderName)
{
    mCurrentPostShader = getShader(shaderName);
}

void RenderManager::setPostShaderTexture(string textureName)
{
    mPostShaderTexture = mPreLoader->getTexture(textureName);
}

void RenderManager::setup(Vec2i windowSize)
{
    mBasicShader = getShader("default_texture");
    
    setWindowSize(windowSize);
}

void RenderManager::setWindowSize(Vec2i windowSize)
{
    mWindowSize = windowSize;
    
    if (fbo_size != windowSize) {
        fbo_size = windowSize;
        gl::Fbo::Format fbo_format;
        fbo_format.enableDepthBuffer(true);
        fbo_format.enableMipmapping(true);
        fbo_format.setMinFilter(GL_NEAREST);
        fbo_format.setMagFilter(GL_NEAREST);
        ca_write_fbo = gl::Fbo(fbo_size.x, fbo_size.y, fbo_format);
        ca_read_fbo  = gl::Fbo(fbo_size.x, fbo_size.y, fbo_format);

        /*
        {   
            gl::SaveFramebufferBinding fbo_save;

            ca_read_fbo.bindFramebuffer();
            gl::clear( mBGColor );
            ca_read_fbo.unbindFramebuffer();

            ca_write_fbo.bindFramebuffer();
            gl::clear( mBGColor ); 
            ca_write_fbo.unbindFramebuffer();
        }
        */
        
        mPostCamera.setOrtho(0, windowSize.x, 0, windowSize.y, -1, 1);
    }
}

gl::Texture RenderManager::getReadBuffer()
{
    return ca_read_fbo.getTexture();
}

void RenderManager::update()
{
    // CA Calculation Pass: Draw to Write Buffer
    {
        gl::SaveFramebufferBinding fbo_save;
        
        // Bind Write Buffer
    	ca_write_fbo.bindFramebuffer();
        
        mBasicShader.bind();
        mBasicShader.uniform("u_tex", 0);
        mBasicShader.uniform("u_params_tex", 1);
        mBasicShader.uniform("u_mvp_matrix", mPostCamera.getProjectionMatrix() * mPostCamera.getModelViewMatrix());
    
        // Draw the Read Buffer
        {
            gl::Texture texture = ca_read_fbo.getTexture();
            gl::SaveTextureBindState saveBindState( texture.getTarget() );
            texture.bind();
            
            Rectf destRect = texture.getCleanBounds();
            const Area srcArea = Area( destRect );

            vector<Vec2f> positions;
    
            positions.push_back(Vec2f(destRect.getX2(), destRect.getY1()));
            positions.push_back(Vec2f(destRect.getX1(), destRect.getY1()));
            positions.push_back(Vec2f(destRect.getX2(), destRect.getY2()));
            positions.push_back(Vec2f(destRect.getX1(), destRect.getY2()));

            mVBO->get("a_position")->setData(positions);
            
            vector<Vec2f> texCoords;     
                       
            const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
            texCoords.push_back(Vec2f(srcCoords.getX2(), srcCoords.getY1()));
            texCoords.push_back(Vec2f(srcCoords.getX1(), srcCoords.getY1()));
            texCoords.push_back(Vec2f(srcCoords.getX2(), srcCoords.getY2()));
            texCoords.push_back(Vec2f(srcCoords.getX1(), srcCoords.getY2()));
            
            mVBO->get("a_texcoord")->setData(texCoords);
            
            mVBO->draw(mBasicShader);
            
            glViewport(0,0,mWindowSize.x,mWindowSize.y);
            gl::clear(mBGColor);
            
            for(int i=0; i<LAYER_INVALID; i++)
            {
                RenderLayer currentLayer = (RenderLayer) i;       
                mCallbacks[currentLayer].call(true);
            }
        }

        mBasicShader.unbind();
        
    }
    
    // Swap Framebuffers
    gl::Fbo tmp = ca_read_fbo;
    ca_read_fbo = ca_write_fbo;
    ca_write_fbo = tmp;
}

void RenderManager::draw()
{  
    mCurrentPostShader.bind();
        
    mCurrentPostShader.uniform("u_screenTex", 0);
    mCurrentPostShader.uniform("u_secondaryTex", 1);
    mCurrentPostShader.uniform("u_mvp_matrix",  mPostCamera.getProjectionMatrix() * mPostCamera.getModelViewMatrix());
    mCurrentPostShader.uniform("u_postAlpha", mPostShaderAlpha);
    
    // Draw the Read Buffer
    {
        gl::Texture texture = ca_read_fbo.getTexture();
        gl::SaveTextureBindState saveBindState( texture.getTarget() );
        texture.bind(0);
        
        if (mPostShaderTexture)
            mPostShaderTexture.bind(1);
        
        Rectf destRect = texture.getCleanBounds();
        const Area srcArea = Area( destRect );

        vector<Vec2f> positions;
    
        positions.push_back(Vec2f(destRect.getX2(), destRect.getY1()));
        positions.push_back(Vec2f(destRect.getX1(), destRect.getY1()));
        positions.push_back(Vec2f(destRect.getX2(), destRect.getY2()));
        positions.push_back(Vec2f(destRect.getX1(), destRect.getY2()));

        mPostVBO->get("a_position")->setData(positions);
        
        vector<Vec2f> texCoords;     
                   
        const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
        texCoords.push_back(Vec2f(srcCoords.getX2(), srcCoords.getY1()));
        texCoords.push_back(Vec2f(srcCoords.getX1(), srcCoords.getY1()));
        texCoords.push_back(Vec2f(srcCoords.getX2(), srcCoords.getY2()));
        texCoords.push_back(Vec2f(srcCoords.getX1(), srcCoords.getY2()));
            
        mPostVBO->get("a_texcoord")->setData(texCoords);
        
        mPostVBO->draw(mCurrentPostShader);
    }
        
    mCurrentPostShader.unbind();
}

}} // namespace bloom::effects
