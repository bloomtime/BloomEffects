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

// Include header
#include "ciJson.h"

// Import namespace
using namespace std;

namespace cinder
{

	namespace json
	{

		// Add values
		void append(Value & object, const string & key, bool value) { object[key] = Value(value); }
		void append(Value & object, const string & key, const char * value) { object[key] = Value(value); }
		void append(Value & object, const string & key, double value) { object[key] = Value(value); }
		void append(Value & object, const string & key, float value) { object[key] = Value(value); }
		void append(Value & object, const string & key, int32_t value) { object[key] = Value(value); }
		void append(Value & object, const string & key, string value) { object[key] = Value(value.c_str()); }
		void append(Value & object, const string & key, uint32_t value) { object[key] = Value(value); }

		// Append list of values
		void append(Value & object, const string & key, vector<bool> values)
		{
			for (vector<bool>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<const char *> values)
		{
			for (vector<const char *>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<double> values)
		{
			for (vector<double>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<float> values)
		{
			for (vector<float>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<int32_t> values)
		{ 
			for (vector<int32_t>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<string> values) 
		{ 
			for (vector<string>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<uint32_t> values)
		{ 
			for (vector<uint32_t>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}
		void append(Value & object, const string & key, vector<Value> values) 
		{
			for (vector<Value>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
				object[key].append(* memberIt);
		}

		// Deserialize JSON string to object
		Value deserialize(const string & value)
		{

			// Parse and return data
			Json::Reader reader;
			Json::Value root;
			reader.parse(value, root);
			return root;

		}

		// Load JSON from file
		Value load(const string & filename)
		{

			// Open config file, starting from end
			fstream file;
			file.open(filename.c_str(), ios::in|ios::ate);
			if (!file.is_open())
			{
				Value value;
				return value;
			}

			// Get file size and rewind to beginning
			ifstream::pos_type size = file.tellg();
			char * configData = new char[size];
			file.seekg(0, ios::beg);

			// Read entire file into JSON data
			file.read(configData, size);
			Value value = deserialize(string(configData));

			// Clean up
			delete [] configData;
			file.close();

			// Return root
			return value;

		}

		// Load JSON from URL
		Value loadUrl(const string & url, bool isEscaped)
		{

			// Load URL
			DataSourceRef dataSource = ci::loadUrl(Url(url, isEscaped));

			// Read buffer
			Buffer buffer;
			try
			{
				buffer = dataSource->getBuffer();
			}
			catch (...)
			{
				Value value;
				return value;
			}
			size_t dataSize = buffer.getDataSize();
			shared_ptr<int_fast8_t> bufferStr(new int_fast8_t[dataSize + 1], checked_array_deleter<int_fast8_t>());
			memcpy(bufferStr.get(), buffer.getData(), buffer.getDataSize());
			bufferStr.get()[dataSize] = 0;

			// Convert buffer to string, deserialize and return
			return deserialize(string(reinterpret_cast<char *>(bufferStr.get())));

		}

		// Read values (index)
		bool readBool(Value & object, int32_t index) { return object[index].asBool(); }
		bool readBool(const Value & object, int32_t index) { return object[index].asBool(); }
		const char * readCString(Value & object, int32_t index) { return object[index].asCString(); }
		const char * readCString(const Value & object, int32_t index) { return object[index].asCString(); }
		double readDouble(Value & object, int32_t index) { return object[index].asDouble(); }
		double readDouble(const Value & object, int32_t index) { return object[index].asDouble(); }
		int32_t readInt(Value & object, int32_t index) { return object[index].asInt(); }
		int32_t readInt(const Value & object, int32_t index) { return object[index].asInt(); }
		string readString(Value & object, int32_t index) { return object[index].asString(); }
		string readString(const Value & object, int32_t index) { return object[index].asString(); }
		uint32_t readUint(Value & object, int32_t index) { return object[index].asUInt(); }
		uint32_t readUint(const Value & object, int32_t index) { return object[index].asUInt(); }
		Value readValue(Value & object, int32_t index) { return object[index]; }
		Value readValue(const Value & object, int32_t index) { return object[index]; }

		// Read values (index, plural)
		vector<bool> readBools(Value & object, int32_t index) 
		{
			vector<bool> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asBool());
			return members;
		}
		vector<bool> readBools(const Value & object, int32_t index) 
		{
			vector<bool> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asBool());
			return members;
		}
		vector<const char *> readCStrings(Value & object, int32_t index) 
		{
			vector<const char *> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asCString());
			return members;
		}
		vector<const char *> readCStrings(const Value & object, int32_t index) 
		{
			vector<const char *> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asCString());
			return members;
		}
		vector<double> readDoubles(Value & object, int32_t index) 
		{
			vector<double> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asDouble());
			return members;
		}
		vector<double> readDoubles(const Value & object, int32_t index) 
		{
			vector<double> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asDouble());
			return members;
		}
		vector<int32_t> readInts(Value & object, int32_t index) 
		{
			vector<int32_t> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asInt());
			return members;
		}
		vector<int32_t> readInts(const Value & object, int32_t index) 
		{
			vector<int32_t> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asInt());
			return members;
		}
		vector<string> readStrings(Value & object, int32_t index) 
		{
			vector<string> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asString());
			return members;
		}
		vector<string> readStrings(const Value & object, int32_t index) 
		{
			vector<string> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asString());
			return members;
		}
		vector<uint32_t> readUints(Value & object, int32_t index) 
		{
			vector<uint32_t> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asUInt());
			return members;
		}
		vector<uint32_t> readUints(const Value & object, int32_t index) 
		{
			vector<uint32_t> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i].asUInt());
			return members;
		}
		vector<Value> readValues(Value & object, int32_t index) 
		{ 
			vector<Value> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i]);
			return members;
		}
		vector<Value> readValues(const Value & object, int32_t index) 
		{ 
			vector<Value> members;
			for (uint32_t i = 0; i < object[index].size(); i++)
				members.push_back(object[index][i]);
			return members;
		}

		// Read values (key)
		bool readBool(Value & object, const string & key) { return object[key].asBool(); }
		bool readBool(const Value & object, const string & key) { return object[key].asBool(); }
		const char * readCString(Value & object, const string & key) { return object[key].asCString(); }
		const char * readCString(const Value & object, const string & key) { return object[key].asCString(); }
		double readDouble(Value & object, const string & key) { return object[key].asDouble(); }
		double readDouble(const Value & object, const string & key) { return object[key].asDouble(); }
		int32_t readInt(Value & object, const string & key) { return object[key].asInt(); }
		int32_t readInt(const Value & object, const string & key) { return object[key].asInt(); }
		string readString(Value & object, const string & key) { return object[key].asString(); }
		string readString(const Value & object, const string & key) { return object[key].asString(); }
		uint32_t readUint(Value & object, const string & key) { return object[key].asUInt(); }
		uint32_t readUint(const Value & object, const string & key) { return object[key].asUInt(); }
		Value readValue(Value & object, const string & key) { return object[key]; }
		Value readValue(const Value & object, const string & key) { return object[key]; }

		// Read values (key, plural)
		vector<bool> readBools(Value & object, const string & key) 
		{
			vector<bool> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asBool());
			return members;
		}
		vector<bool> readBools(const Value & object, const string & key) 
		{
			vector<bool> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asBool());
			return members;
		}
		vector<const char *> readCStrings(Value & object, const string & key) 
		{
			vector<const char *> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asCString());
			return members;
		}
		vector<const char *> readCStrings(const Value & object, const string & key) 
		{
			vector<const char *> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asCString());
			return members;
		}
		vector<double> readDoubles(Value & object, const string & key) 
		{
			vector<double> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asDouble());
			return members;
		}
		vector<double> readDoubles(const Value & object, const string & key) 
		{
			vector<double> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asDouble());
			return members;
		}
		vector<int32_t> readInts(Value & object, const string & key) 
		{
			vector<int32_t> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asInt());
			return members;
		}
		vector<int32_t> readInts(const Value & object, const string & key) 
		{
			vector<int32_t> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asInt());
			return members;
		}
		vector<string> readStrings(Value & object, const string & key) 
		{
			vector<string> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asString());
			return members;
		}
		vector<string> readStrings(const Value & object, const string & key) 
		{
			vector<string> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asString());
			return members;
		}
		vector<uint32_t> readUints(Value & object, const string & key) 
		{
			vector<uint32_t> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asUInt());
			return members;
		}
		vector<uint32_t> readUints(const Value & object, const string & key) 
		{
			vector<uint32_t> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i].asUInt());
			return members;
		}
		vector<Value> readValues(Value & object, const string & key) 
		{ 
			vector<Value> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i]);
			return members;
		}
		vector<Value> readValues(const Value & object, const string & key) 
		{ 
			vector<Value> members;
			for (uint32_t i = 0; i < object[key].size(); i++)
				members.push_back(object[key][i]);
			return members;
		}

		// Save JSON from file
		void save(const Value & value, const string & filename)
		{

			// Serialize data into string
			string jsonStr = serialize(value);

			// Write JSON to file
			fstream file;
			file.open(filename.c_str(), ios::out|ios::trunc);
			file << jsonStr;
			file.close();

		}

#if defined(CINDER_MSW)

		// POST JSON to URL
		string saveUrl(const Value & value, const string & url)
		{

			// Tokenize URL
			vector<string> tokens;
			boost::split(tokens, url, boost::is_any_of("/"));
			if (tokens.size() < 3)
				return "Invalid URL. Please include protocol.";

			// Get server portion of URL
			string server = tokens[2];

			// Remove server portion of URL
			for (int32_t i = 0; i < 3; i++)
				tokens.erase(tokens.begin());

			// Get action portion of URL
			string action = "";
			for (vector<string>::const_iterator tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt)
				action += "/" + * tokenIt;

			// Serialize data
			string data = serialize(value);

			// Open Internet connection
			HINTERNET session = InternetOpenA("WinInetConnection", INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
			if (session == NULL)
				return "Unable to establish Internet session.";

			// Open server
			HINTERNET connection = InternetConnectA(session, server.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
			if (connection == NULL)
				return "Unable to establish Internet connection.";

			// Open request flags
			DWORD mOpenRequestFlags = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
				INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
				INTERNET_FLAG_KEEP_CONNECTION |
				INTERNET_FLAG_NO_AUTO_REDIRECT |
				INTERNET_FLAG_NO_COOKIES |
				INTERNET_FLAG_NO_CACHE_WRITE |
				INTERNET_FLAG_NO_UI |
				INTERNET_FLAG_RELOAD;

			// Open request
			HINTERNET request = HttpOpenRequestA(connection, "POST", action.c_str(), "HTTP/1.0", NULL, NULL, mOpenRequestFlags, 0);
			if (request == NULL)
				return "Unable to create request.";

			// Send request
			int_fast8_t buffer;
			DWORD size;
			string headers = "Content-Type: application/x-www-form-urlencoded";
			string response = "";
			if (HttpSendRequestA(request, headers.c_str(), headers.length(), (LPVOID)(data.c_str()), data.length()))
			{

				// Read request into buffer
				while (InternetReadFile(request, &buffer, 1, &size))
				{
					if (size != 1)
						break;
					response += buffer;
				}

			}

			// Close Internet handles
			InternetCloseHandle(request);
			InternetCloseHandle(connection);
			InternetCloseHandle(session);

			// Return response
			return response;

		}
#endif

		// Serialize JSON object to string
		string serialize(const Json::Value & value)
		{	

			// Writes JSON
			Json::StyledWriter writer;

			// Convert value to stylized string and return
			return writer.write(value);

		}

	}

}
