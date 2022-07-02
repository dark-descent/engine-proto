#include "Engine.hpp"
#include "node.h"

Engine* Engine::engine_ = nullptr;

Engine::Engine() : renderer(*this), systemsStack_(), systems_(), config()
{
	loadSystem(&renderer);
}

Engine::~Engine()
{
	
}

void Engine::initialize(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	v8::Isolate* isolate = info.GetIsolate();

	if (engine_ != nullptr)
	{
		isolate->ThrowException(createException(isolate, "Engine is already initialized!"));
		return;
	}

	if (info.Length() != 1)
	{
		isolate->ThrowException(createException(isolate, "Missing config argument!"));
		return;
	}

	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Value> config = info[0];

	v8::Local<v8::Value> testVal = config->ToObject(ctx).ToLocalChecked()->Get(ctx, createString(isolate, "test")).ToLocalChecked();

	node::AddEnvironmentCleanupHook(isolate, Engine::cleanup, isolate);

	engine_ = new Engine();

	info.GetReturnValue().Set(testVal);
}

void Engine::destory(const v8::FunctionCallbackInfo<v8::Value>& info)
{

}

void Engine::cleanup(void* data)
{
	delete Engine::engine_;
	Engine::engine_ = nullptr;
	v8::Isolate* isolate = static_cast<v8::Isolate*>(data);
	node::RemoveEnvironmentCleanupHook(isolate, Engine::cleanup, isolate);
}