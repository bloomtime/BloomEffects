//
//  EmitVolume.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <vector>
#include <list>
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>
#include "cinder/Vector.h"

enum EmitVolume {
    VOLUME_POINT,
    VOLUME_SPHERE,
    VOLUME_BOX,
    VOLUME_CYLINDER
};

// add new Emit Volumes here
const boost::unordered_map<std::string, EmitVolume> EMIT_VOLUMES = boost::assign::map_list_of
    ("Point", VOLUME_POINT)
    ("Sphere", VOLUME_SPHERE)
    ("Box", VOLUME_BOX)
    ("Cylinder", VOLUME_CYLINDER);

class EmissionVolume {
public:
    EmissionVolume() : 
        mVolumeType(VOLUME_SPHERE),
        mScale(ci::Vec3f(0.0f, 0.0f, 0.0f))
        {}
        
    ~EmissionVolume() {}
    
    void setVolumeType(EmitVolume type) { mVolumeType = type; }
    void setScale(ci::Vec3f scale) { mScale = scale; }
    
    ci::Vec3f getRandomPoint(float scale=1.0f);
    
protected:
    EmitVolume mVolumeType;
    ci::Vec3f mScale;
};
