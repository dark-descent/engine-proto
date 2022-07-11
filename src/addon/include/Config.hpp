#pragma once

struct Config
{
	
	public:
		// std::string gameName;

		Config(v8::Local<v8::Object> config)
		{
			v8::Isolate* isolate = config->GetIsolate();
			// v8::Local<v8::Value> gameNameVal = config->Get(isolate->GetCurrentContext(), createString(isolate, "gameName")).ToLocalChecked();
			// v8::String::Utf8Value utf8GameName(isolate, gameNameVal);
			// gameName = std::string(*utf8GameName);
		}
};