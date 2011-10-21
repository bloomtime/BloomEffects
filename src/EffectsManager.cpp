#include "EffectsManager.h"

const string FX_EXTENSION = ".effect.json";

EffectsManager::EffectsManager()
{
}

EffectsManager::~EffectsManager()
{
    //necessary?
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    
	for( list<Effect *>::const_iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        delete (*it);
    }
    
    mCamera = NULL;
}

void EffectsManager::setup()
{
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    glGenFramebuffers(1, &m_framebuffer); 
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer); 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);
    glViewport(0, 0, getWindowWidth(), getWindowHeight());

    mTestEffect = new Effect("test.effect.json");
    mTestEffect->setCamera(mCamera);
    mTestEffect->setup();
    
    mTestEffect->start();
}


void EffectsManager::createEffect(string effectName, bool start)
{
    Effect *newEffect = new Effect(effectName + FX_EXTENSION);
    newEffect->setCamera(mCamera);
    newEffect->setup();
    
    mEffects.push_back(newEffect);
    
    if (start)
        newEffect->start();
}

void EffectsManager::stopEffect(bool hardStop)
{
    // pass around fx pointer or uid?
}


void EffectsManager::update()
{
	for( list<Effect *>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if (!(*it)->isStopped())
        {
            (*it)->update();
        }
        else
        {
            delete (*it);  
            it = mEffects.erase(it);
        }
    }
    
	//gl::enableDepthRead();
	//gl::enableDepthWrite();
}

void EffectsManager::draw()
{  
	for( list<Effect *>::const_iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if (!(*it)->isStopped())
        {
            (*it)->draw();
        }
    }
}
