//
//  EffectAttribute.cpp
//  CinderFizz
//
//  Created by Tom Carden on 11/3/11.
//  Copyright (c) 2011 Bloom Studio, Inc. All rights reserved.
//

#include "EffectAttribute.h"

// init static variables
boost::unordered_map<string, gl::Texture> EffectAttribute::sTextureCache;
boost::unordered_map<string, gl::GlslProg> EffectAttribute::sShaderCache;

