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
    newAttr.name = attrName;
    newAttr.type = attrType;
    
    mAttributes[attrName] = newAttr;
}