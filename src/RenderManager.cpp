//
//  RenderManager.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "RenderManager.h"

using namespace std;
using namespace ci;

RenderManagerRef RenderManager::create()
{
    return RenderManagerRef( new RenderManager() );
}

RenderManager::RenderManager()
{
     mBGColor = Color(0.5f, 0.8f, 0.7f);
     
     mPostShaderName = DEFAULT_POST_SHADER;
}

RenderManager::~RenderManager()
{
}

void RenderManager::setPostShader(string shaderName)
{
    //TODO need to cache here, possibly using EffectAttribute
    mCurrentPostShader = gl::GlslProg(app::App::loadResource(shaderName + "_vert.glsl"), app::App::loadResource(shaderName + "_frag.glsl"));
    vtx_handle_post = mCurrentPostShader.getAttribLocation("a_position");
    txc_handle_post = mCurrentPostShader.getAttribLocation("a_texcoord");
    mPostShaderName = shaderName;
}

void RenderManager::setup(EffectsStateRef fxState, Vec2i windowSize)
{
    mState = fxState;
    
    mBasicShader = gl::GlslProg(app::App::loadResource("default_texture_vert.glsl"), app::App::loadResource("default_texture_frag.glsl"));
    vtx_handle = mBasicShader.getAttribLocation("a_position");
    txc_handle = mBasicShader.getAttribLocation("a_texcoord");
    
    setPostShader(mState->getPostShader());
    mDefaultPostShader = mCurrentPostShader;

    setWindowSize(windowSize);
}

void RenderManager::setWindowSize(Vec2i windowSize)
{
    if (fbo_size != windowSize) {
        fbo_size = windowSize;
        gl::Fbo::Format fbo_format;
        fbo_format.enableDepthBuffer(true);
        fbo_format.enableMipmapping(true);
        fbo_format.setMinFilter(GL_NEAREST);
        fbo_format.setMagFilter(GL_NEAREST);
        ca_write_fbo = gl::Fbo(fbo_size.x, fbo_size.y, fbo_format);
        ca_read_fbo  = gl::Fbo(fbo_size.x, fbo_size.y, fbo_format);

//        {   
//            gl::SaveFramebufferBinding fbo_save;
//
//            ca_read_fbo.bindFramebuffer();
//            gl::clear( mBGColor );
//    //        ca_read_fbo.unbindFramebuffer();
//
//            ca_write_fbo.bindFramebuffer();
//            gl::clear( mBGColor ); 
//    //        ca_write_fbo.unbindFramebuffer();
//        }

        mPostCamera.setOrtho(0, windowSize.x, 0, windowSize.y, -1, 1);
    }
}

void RenderManager::update(list<EffectWeakRef> effects, list<EffectRef> oneOffEffects)
{
    string stateShader = mState->getPostShader();
    if (stateShader != mPostShaderName)
    {
      setPostShader(stateShader);
    }
    
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

            GLfloat verts[8];
            glVertexAttribPointer( vtx_handle, 2, GL_FLOAT, GL_FALSE, 0, verts );
            glEnableVertexAttribArray( vtx_handle );
            GLfloat texCoords[8];
            glVertexAttribPointer( txc_handle, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
            glEnableVertexAttribArray( txc_handle );

            verts[0*2+0] = destRect.getX2(); verts[0*2+1] = destRect.getY1();
            verts[1*2+0] = destRect.getX1(); verts[1*2+1] = destRect.getY1();
            verts[2*2+0] = destRect.getX2(); verts[2*2+1] = destRect.getY2();
            verts[3*2+0] = destRect.getX1(); verts[3*2+1] = destRect.getY2();

            const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
            texCoords[0*2+0] = srcCoords.getX2(); texCoords[0*2+1] = srcCoords.getY1();
            texCoords[1*2+0] = srcCoords.getX1(); texCoords[1*2+1] = srcCoords.getY1();
            texCoords[2*2+0] = srcCoords.getX2(); texCoords[2*2+1] = srcCoords.getY2();
            texCoords[3*2+0] = srcCoords.getX1(); texCoords[3*2+1] = srcCoords.getY2();

            glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
            
            gl::clear( mBGColor ); 
            
            //TODO HERE REGISTERED RENDERLAYER DRAW CALLS
            /*
            for( list<EffectWeakRef>::const_iterator it = effects.begin(); it != effects.end(); ++it )
            {
                EffectWeakRef current = (*it);
                
                if ( EffectRef currentEffect = current.lock()) 
                {
                    if (!currentEffect->isStopped())
                    {
                        currentEffect->draw();
                    }
                }
            }
            
            for( list<EffectRef>::const_iterator it = oneOffEffects.begin(); it != oneOffEffects.end(); ++it )
            {
                if (!(*it)->isStopped())
                {
                    (*it)->draw();
                }
            }
            */
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
    mCurrentPostShader.uniform("u_mvp_matrix",  mPostCamera.getProjectionMatrix() * mPostCamera.getModelViewMatrix());
    mCurrentPostShader.uniform("u_postAlpha", mState->getPostShaderAlpha());
    
    // Draw the Read Buffer
    {
        gl::Texture texture = ca_read_fbo.getTexture();
        gl::SaveTextureBindState saveBindState( texture.getTarget() );
        texture.bind();
        
        Rectf destRect = texture.getCleanBounds();
        const Area srcArea = Area( destRect );

        GLfloat verts[8];
        glVertexAttribPointer( vtx_handle, 2, GL_FLOAT, GL_FALSE, 0, verts );
        glEnableVertexAttribArray( vtx_handle );
        GLfloat texCoords[8];
        glVertexAttribPointer( txc_handle, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
        glEnableVertexAttribArray( txc_handle );

        verts[0*2+0] = destRect.getX2(); verts[0*2+1] = destRect.getY1();
        verts[1*2+0] = destRect.getX1(); verts[1*2+1] = destRect.getY1();
        verts[2*2+0] = destRect.getX2(); verts[2*2+1] = destRect.getY2();
        verts[3*2+0] = destRect.getX1(); verts[3*2+1] = destRect.getY2();

        const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
        texCoords[0*2+0] = srcCoords.getX2(); texCoords[0*2+1] = srcCoords.getY1();
        texCoords[1*2+0] = srcCoords.getX1(); texCoords[1*2+1] = srcCoords.getY1();
        texCoords[2*2+0] = srcCoords.getX2(); texCoords[2*2+1] = srcCoords.getY2();
        texCoords[3*2+0] = srcCoords.getX1(); texCoords[3*2+1] = srcCoords.getY2();

        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    }
    mCurrentPostShader.unbind();
}

