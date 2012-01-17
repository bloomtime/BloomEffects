//
//  EmitVolume.cpp
//  Biologic
//
//  Created by Tom Carden on 12/21/11.
//  Copyright (c) 2011 Bloom Studio, Inc. All rights reserved.
//

#include "EmitVolume.h"
#include "cinder/Rand.h"

namespace bloom { namespace effects {

using namespace ci;
using namespace std;

Vec3f EmissionVolume::getRandomPoint(float scale)
{
    Vec3f finalScale = mScale * scale;
    
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
            randomPoint = Rand::randVec3f() * Rand::randFloat(finalScale[1], finalScale[0]);
            break;
        }
        case VOLUME_CYLINDER:
        {
            // x is radius of cylinder, y is inner radius, z is cylinder height
            // if y is 0, then the cylinder is "solid", otherwise there is a hole of size y in the center
            Vec2f xz = Rand::randVec2f() * Rand::randFloat(finalScale[1], finalScale[0]);
            randomPoint = Vec3f(xz[0], xz[1], Rand::randFloat(0.0f, finalScale[2]));
            break;
        }
        case VOLUME_BOX:
        {
            // center of box is at emitter position for now.  may want to change this to a corner later but not sure
            float randomX = Rand::randFloat(0.0f, finalScale[0]) - finalScale[0] * .5f;
            float randomY = Rand::randFloat(0.0f, finalScale[1]) - finalScale[1] * .5f;
            float randomZ = Rand::randFloat(0.0f, finalScale[2]) - finalScale[2] * .5f;
            randomPoint = Vec3f(randomX, randomY, randomZ);
            break;
        }
        default:
            break;
    }
    return randomPoint;
}

}} // namespace bloom::effects
