//
//  EffectEvent.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include <string>

class EffectsState;

typedef std::shared_ptr<EffectsState> EffectsStateRef;

const string DEFAULT_POST_SHADER = "defaultPost";

class EffectsState
{
public:
    static EffectsStateRef create() { return EffectsStateRef( new EffectsState() ); }

    ~EffectsState() {}
    
    void setPostShader(string shaderName) { mPostShader = shaderName; }
    string getPostShader() { return mPostShader; }
    void setDefaultPostShader() { mPostShader = DEFAULT_POST_SHADER; }
    
    void setPostShaderAlpha(float alpha) { mPostShaderAlpha = alpha; }
    float getPostShaderAlpha() { return mPostShaderAlpha; }
    
protected:    
    string mPostShader;
    float mPostShaderAlpha;
    
private:
    EffectsState() :
        mPostShader("defaultPost"),
        mPostShaderAlpha(1.0f)
    {
    }
};
