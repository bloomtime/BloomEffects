//
//  EmitVolume.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "cinder/Rand.h"
#include <vector>
#include <list>

using namespace ci;
using namespace std;

enum EmitVolume {
    VOLUME_POINT,
    VOLUME_SPHERE,
    VOLUME_BOX,
    VOLUME_CYLINDER
};

// add new Emit Volumes here
const boost::unordered_map<string, EmitVolume> EMIT_VOLUMES = boost::assign::map_list_of
    ("Point", VOLUME_POINT)
    ("Sphere", VOLUME_SPHERE)
    ("Box", VOLUME_BOX)
    ("Cylinder", VOLUME_CYLINDER);

class EmissionVolume {
public:
    EmissionVolume() : 
        mVolumeType(VOLUME_SPHERE),
        mScale(Vec3f(0.0f, 0.0f, 0.0f))
        {}
        
    ~EmissionVolume() {}
    
    void setVolumeType(EmitVolume type) { mVolumeType = type; }
    void setScale(Vec3f scale) { mScale = scale; }
    
    Vec3f getRandomPoint()
    {
        Vec3f randomPoint = Vec3f(0.0f, 0.0f, 0.0f);
        switch (mVolumeType) {
            case VOLUME_POINT:
            {
                randomPoint = Vec3f(0.0f, 0.0f, 0.0f);
                break;
            }
            case VOLUME_SPHERE:
            {
                // x is radius of cylinder, y is inner radius
                // if y is 0, then the sphere is "solid", otherwise there is a hole of size y in the center
                randomPoint = Rand::randVec3f() * Rand::randFloat(mScale[1], mScale[0]);
                break;
            }
            case VOLUME_CYLINDER:
            {
                // x is radius of cylinder, y is inner radius, z is cylinder height
                // if y is 0, then the cylinder is "solid", otherwise there is a hole of size y in the center
                Vec2f xz = Rand::randVec2f() * Rand::randFloat(mScale[1], mScale[0]);
                randomPoint = Vec3f(xz[0], Rand::randFloat(0.0f, mScale[2]), xz[1]);
                break;
            }
            case VOLUME_BOX:
            {
                // center of box is at emitter position for now.  may want to change this to a corner later but not sure
                float randomX = Rand::randFloat(0.0f, mScale[0]) - mScale[0] * .5f;
                float randomY = Rand::randFloat(0.0f, mScale[1]) - mScale[1] * .5f;
                float randomZ = Rand::randFloat(0.0f, mScale[2]) - mScale[2] * .5f;
                randomPoint = Vec3f(randomX, randomY, randomZ);
                break;
            }
            default:
                break;
        }
        return randomPoint;
    }
    
protected:
    EmitVolume mVolumeType;
    Vec3f mScale;
};
