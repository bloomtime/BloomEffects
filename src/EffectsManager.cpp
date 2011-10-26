#include "EffectsManager.h"

const string FX_EXTENSION = ".effect.json";

EffectsManagerRef EffectsManager::create()
{
    return EffectsManagerRef( new EffectsManager() );
}

EffectsManager::EffectsManager()
{
    mCamera = NULL;
}

EffectsManager::~EffectsManager()
{
    glDeleteRenderbuffers(1, &m_renderbuffer);
    glDeleteFramebuffers(1, &m_framebuffer);
    
	for( list<EffectRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        //(*it) = NULL;
        it = mEffects.erase(it);
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
}


EffectRef EffectsManager::createEffect(string effectName, bool start)
{
    EffectRef newEffect = Effect::createFromPath(effectName + FX_EXTENSION);
    newEffect->setCamera(mCamera);
    newEffect->setup();
    
    mEffects.push_back(newEffect);
    
    if (start)
        newEffect->start();
        
    return newEffect;
}

void EffectsManager::destroyEffect(EffectRef effect, bool hardStop)
{
	for( list<EffectRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if ((*it) == effect)
        {
            (*it)->stop(hardStop);
            it = mEffects.erase(it);
        }
    }
}

void EffectsManager::update()
{
	for( list<EffectRef>::iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if (!(*it)->isStopped())
        {
            (*it)->update();
        }
        else
        {
            it = mEffects.erase(it);
        }
    }
    
	//gl::enableDepthRead();
	//gl::enableDepthWrite();
}

void EffectsManager::draw()
{  
	for( list<EffectRef>::const_iterator it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        if (!(*it)->isStopped())
        {
            (*it)->draw();
        }
    }
}
