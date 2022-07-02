#include "Engine.hpp"

Engine* Engine::engine = nullptr;

Engine::Engine()
{
	
}

void Engine::initialize(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	v8::Isolate* isolate = info.GetIsolate();
	
	if (engine != nullptr)
	{
		isolate->ThrowException(createException(isolate, "Engine is already initialized!"));
		return;
	}

	if(info.Length() != 1)
	{
		isolate->ThrowException(createException(isolate, "Missing config argument!"));
		return;
	}

	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Value> config = info[0];

	v8::Local<v8::Value> testVal = config->ToObject(ctx).ToLocalChecked()->Get(ctx, createString(isolate, "test")).ToLocalChecked();

	info.GetReturnValue().Set(testVal);
}

void Engine::destory(const v8::FunctionCallbackInfo<v8::Value>& info)
{

}