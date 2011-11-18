#include "EffectsRenderer.h"

EffectsRendererRef EffectsRenderer::create()
{
    return EffectsRendererRef( new EffectsRenderer() );
}

EffectsRenderer::EffectsRenderer()
{
     mBGColor = Color(1.0f, 1.0f, 1.0f);
     mPostShaderAlpha = 1.0f; 
     
     mPostShaderName = "defaultPost";
}

EffectsRenderer::~EffectsRenderer()
{
}

void EffectsRenderer::setPostShader(string shaderName)
{
    mPostShaderName = shaderName;
}

void EffectsRenderer::setPostShaderAlpha(float alpha)
{
    mPostShaderAlpha = alpha;
}

void EffectsRenderer::setup()
{
    prog = gl::GlslProg(app::App::loadResource("default_texture_vert.glsl"), app::App::loadResource("default_texture_frag.glsl"));
    vtx_handle = prog.getAttribLocation("a_position");
    txc_handle = prog.getAttribLocation("a_texcoord");
    prog_post = gl::GlslProg(app::App::loadResource(mPostShaderName + "_vert.glsl"), app::App::loadResource(mPostShaderName + "_frag.glsl"));
    vtx_handle_post = prog_post.getAttribLocation("a_position");
    txc_handle_post = prog_post.getAttribLocation("a_texcoord");
    
    fbo_size = getWindowSize();
    gl::Fbo::Format fbo_format;
    fbo_format.enableDepthBuffer(false);
    fbo_format.enableMipmapping(false);
    fbo_format.setMinFilter(GL_NEAREST);
    fbo_format.setMagFilter(GL_NEAREST);
    ca_write_fbo = gl::Fbo(fbo_size.x, fbo_size.y, fbo_format);
    ca_read_fbo  = gl::Fbo(fbo_size.x, fbo_size.y, fbo_format);

    ca_read_fbo.bindFramebuffer();
    gl::clear( mBGColor ); 
    ca_read_fbo.unbindFramebuffer();

    mPostCamera.setOrtho(0, getWindowWidth(), 0, getWindowHeight(), -1, 1);
}

void EffectsRenderer::update(list<EffectRef> effects)
{
    // CA Calculation Pass: Draw to Write Buffer
    {
        gl::SaveFramebufferBinding fbo_save;
        
        // Bind Write Buffer
    	ca_write_fbo.bindFramebuffer();
        
        prog.bind();
        prog.uniform("u_tex", 0);
        prog.uniform("u_params_tex", 1);
        prog.uniform("u_mvp_matrix", mPostCamera.getProjectionMatrix() * mPostCamera.getModelViewMatrix());
    
        // Draw the Read Buffer
        {
            ci::gl::Texture texture = ca_read_fbo.getTexture();
            ci::gl::SaveTextureBindState saveBindState( texture.getTarget() );
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
            
            for( list<EffectRef>::const_iterator it = effects.begin(); it != effects.end(); ++it )
            {
                if (!(*it)->isStopped())
                {
                    (*it)->draw();
                }
            }
        }

        prog.unbind();
        
    }
    
    // Swap Framebuffers
    gl::Fbo tmp = ca_read_fbo;
    ca_read_fbo = ca_write_fbo;
    ca_write_fbo = tmp;
}

void EffectsRenderer::draw()
{  
    prog_post.bind();
    prog_post.uniform("u_mvp_matrix",  mPostCamera.getProjectionMatrix() * mPostCamera.getModelViewMatrix());
    prog_post.uniform("u_postAlpha", mPostShaderAlpha);
    
    // Draw the Read Buffer
    {
        ci::gl::Texture texture = ca_read_fbo.getTexture();
        ci::gl::SaveTextureBindState saveBindState( texture.getTarget() );
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
    prog_post.unbind();
}

