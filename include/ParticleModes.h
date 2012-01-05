//
//  ParticleModes.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "EffectEvent.h"
#include "EmitVolume.h"

#include "cinder/gl/GlslProg.h"
#include <vector>
#include <list>

enum Texture2Mode {
    TEX2_NONE,
    TEX2_NORMAL,
    TEX2_DISTORT
};

// add new Texture 2 Modes here
const boost::unordered_map<std::string, Texture2Mode> TEXTURE2_MODES = boost::assign::map_list_of
    ("None", TEX2_NONE)
    ("Normal", TEX2_NORMAL)
    ("Distort", TEX2_DISTORT);
    
// Texture Wrap Modes
const boost::unordered_map<std::string, int> TEXTUREWRAP_MODES = boost::assign::map_list_of
    ("REPEAT", GL_REPEAT)
    ("MIRRORED", GL_MIRRORED_REPEAT)
    ("CLAMP", GL_CLAMP_TO_EDGE);
    
    
enum FaceMode {
    FACE_BILLBOARD,
    FACE_AXIS_X,
    FACE_AXIS_Y,
    FACE_AXIS_Z,
};

// add new Face Modes here
const boost::unordered_map<std::string, FaceMode> FACE_MODES = boost::assign::map_list_of
    ("Billboard", FACE_BILLBOARD)
    ("AxisX", FACE_AXIS_X)
    ("AxisY", FACE_AXIS_Y)
    ("AxisZ", FACE_AXIS_Z);
    
enum EmitMode {
    EMIT_BURST,
    EMIT_CONTINUOUS
};

// add new Emit Modes here
const boost::unordered_map<std::string, EmitMode> EMIT_MODES = boost::assign::map_list_of
    ("Burst", EMIT_BURST)
    ("Continuous", EMIT_CONTINUOUS);

enum BlendMode {
    BLEND_ALPHA,
    BLEND_ADDITIVE,
    BLEND_OPAQUE
};

// add new Blend Modes here
const boost::unordered_map<std::string, BlendMode> BLEND_MODES = boost::assign::map_list_of
    ("AlphaBlend", BLEND_ALPHA)
    ("AdditiveBlend", BLEND_ADDITIVE)
    ("OpaqueBlend", BLEND_OPAQUE);
