/*
* 
* Copyright (c) 2011, Ban the Rewind
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or 
* without modification, are permitted provided that the following 
* conditions are met:
* 
* Redistributions of source code must retain the above copyright 
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following disclaimer in 
* the documentation and/or other materials provided with the 
* distribution.
* 
* Neither the name of the Ban the Rewind nor the names of its 
* contributors may be used to endorse or promote products 
* derived from this software without specific prior written 
* permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
*/

#pragma once

// Includes
#include "boost/algorithm/string.hpp"
#include "cinder/Cinder.h"
#include "cinder/DataSource.h"
#include "cinder/Url.h"
#include <fstream>
#include <stdint.h>
#include "json/json.h"
#include <string>
#if defined(CINDER_MSW)
	#include <stdio.h>
	#include <Windows.h>
	#include <WinInet.h>
#endif

namespace cinder 
{ 

	namespace json
	{

		// Alias for JsonCpp types
		typedef Json::ValueType Type;
		typedef Json::Value Value;

		// Load JSON from file, URL
		Value load(const std::string & path);
		Value loadUrl(const std::string & url, bool isEscaped = false);

		// De/serialize
		Value deserialize(const std::string & value);
		std::string serialize(const Value & value);

		// Append value
		void append(Value & object, const std::string & key, bool value);
		void append(Value & object, const std::string & key, const char * value);
		void append(Value & object, const std::string & key, double value);
		void append(Value & object, const std::string & key, float value);
		void append(Value & object, const std::string & key, int32_t value);
		void append(Value & object, const std::string & key, std::string value);
		void append(Value & object, const std::string & key, uint32_t value);

		// Append list of values
		void append(Value & object, const std::string & key, std::vector<bool> values);
		void append(Value & object, const std::string & key, std::vector<const char *> value);
		void append(Value & object, const std::string & key, std::vector<double> values);
		void append(Value & object, const std::string & key, std::vector<float> values);
		void append(Value & object, const std::string & key, std::vector<int32_t> values);
		void append(Value & object, const std::string & key, std::vector<std::string> values);
		void append(Value & object, const std::string & key, std::vector<uint32_t> values);
		void append(Value & object, const std::string & key, std::vector<Value> values);

		// Read values (index)
		bool readBool(Value & object, int32_t index);
		bool readBool(const Value & object, int32_t index);
		const char * readCString(Value & object, int32_t index);
		const char * readCString(const Value & object, int32_t index);
		double readDouble(Value & object, int32_t index);
		double readDouble(const Value & object, int32_t index);
		float readFloat(Value & object, int32_t index);
		float readFloat(const Value & object, int32_t index);
		int32_t readInt(Value & object, int32_t index);
		int32_t readInt(const Value & object, int32_t index);
		std::string readString(Value & object, int32_t index);
		std::string readString(const Value & object, int32_t index);
		uint32_t readUint(Value & object, int32_t index);
		uint32_t readUint(const Value & object, int32_t index);
		Value readValue(Value & object, int32_t index);
		Value readValue(const Value & object, int32_t index);

		// Read values (index, plural)
		std::vector<bool> readBools(Value & object, int32_t index);
		std::vector<bool> readBools(const Value & object, int32_t index);
		std::vector<const char *> readCStrings(Value & object, int32_t index);
		std::vector<const char *> readCStrings(const Value & object, int32_t index);
		std::vector<double> readDoubles(Value & object, int32_t index);
		std::vector<double> readDoubles(const Value & object, int32_t index);
		std::vector<float> readFloats(Value & object, int32_t index);
		std::vector<float> readFloats(const Value & object, int32_t index);
		std::vector<int32_t> readInts(Value & object, int32_t index);
		std::vector<int32_t> readInts(const Value & object, int32_t index);
		std::vector<std::string> readStrings(Value & object, int32_t index);
		std::vector<std::string> readStrings(const Value & object, int32_t index);
		std::vector<uint32_t> readUints(Value & object, int32_t index);
		std::vector<uint32_t> readUints(const Value & object, int32_t index);
		std::vector<Value> readValues(Value & object, int32_t index);
		std::vector<Value> readValues(const Value & object, int32_t index);

		// Read values (key)
		bool readBool(Value & object, const std::string & key);
		bool readBool(const Value & object, const std::string & key);
		const char * readCString(Value & object, const std::string & key);
		const char * readCString(const Value & object, const std::string & key);
		double readDouble(Value & object, const std::string & key);
		double readDouble(const Value & object, const std::string & key);
		float readFloat(Value & object, const std::string & key);
		float readFloat(const Value & object, const std::string & key);
		int32_t readInt(Value & object, const std::string & key);
		int32_t readInt(const Value & object, const std::string & key);
		std::string readString(Value & object, const std::string & key);
		std::string readString(const Value & object, const std::string & key);
		Value readValue(Value & object, const std::string & key);
		Value readValue(const Value & object, const std::string & key);
		uint32_t readUint(Value & object, const std::string & key);
		uint32_t readUint(const Value & object, const std::string & key);

		// Read values (key, plural)
		std::vector<bool> readBools(Value & object, const std::string & key);
		std::vector<bool> readBools(const Value & object, const std::string & key);
		std::vector<const char *> readCStrings(Value & object, const std::string & key);
		std::vector<const char *> readCStrings(const Value & object, const std::string & key);
		std::vector<double> readDoubles(Value & object, const std::string & key);
		std::vector<double> readDoubles(const Value & object, const std::string & key);
		std::vector<float> readFloats(Value & object, const std::string & key);
		std::vector<float> readFloats(const Value & object, const std::string & key);
		std::vector<int32_t> readInts(Value & object, const std::string & key);
		std::vector<int32_t> readInts(const Value & object, const std::string & key);
		std::vector<std::string> readStrings(Value & object, const std::string & key);
		std::vector<std::string> readStrings(const Value & object, const std::string & key);
		std::vector<Value> readValues(Value & object, const std::string & key);
		std::vector<Value> readValues(const Value & object, const std::string & key);
		std::vector<uint32_t> readUints(Value & object, const std::string & key);
		std::vector<uint32_t> readUints(const Value & object, const std::string & key);

		// Save JSON to file, URL
		void save(const Value & value, const std::string & path);
#if defined(CINDER_MSW)
		std::string saveUrl(const Value & value, const std::string & url);
#endif

	}

}
