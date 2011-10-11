//
//  EffectEvent.cpp
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "EffectEvent.h"
#include <boost/foreach.hpp>

EffectEvent::~EffectEvent()
{    
    mAttributes.clear();
}

void EffectEvent::registerAttribute(string attrName, string attrType)
{
    EffectAttribute newAttr;
    newAttr.mName = attrName;
    newAttr.mType = attrType;
    
    mAttributes[attrName] = newAttr;
}
