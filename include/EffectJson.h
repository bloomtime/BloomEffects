//
//  EffectJson.h
//
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "json/json.h"  
#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/App.h"

#include <string>

using namespace ci;
using namespace std;

namespace bloom { namespace effects {

static Json::Value getJsonData(string effectPath)
{
    DataSourceRef dataSource = app::loadResource(effectPath);
    Buffer buf = dataSource->getBuffer();
	size_t dataSize = buf.getDataSize();
	shared_ptr<char> bufString( new char[dataSize+1], checked_array_deleter<char>() );
	memcpy( bufString.get(), buf.getData(), buf.getDataSize() );
	bufString.get()[dataSize] = 0;
    
	// Parse and return data
	Json::Reader reader;
	Json::Value data;
	reader.parse(bufString.get(), data);
	return data;
}

static vector<Json::Value> readVector(Json::Value object, string key) 
{ 
	vector<Json::Value> members;
	for (uint32_t i = 0; i < object[key].size(); i++)
		members.push_back(object[key][i]);
	return members;
}

}} // namespace bloom::effects