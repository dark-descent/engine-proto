#include "System.hpp"
#include "Engine.hpp"

void System::testEvent(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	System* self = static_cast<System*>(info.Data().As<v8::External>()->Value());
	for(const auto& l : self->eventListeners_)
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Local<v8::String> event = v8::Local<v8::String>::Cast(info[0]);
		v8::String::Utf8Value eventString(isolate, info[0]);
		System* self = static_cast<System*>(info.Data().As<v8::External>()->Value());
		
		self->emitEvent(*eventString, info[1]);
	}
}

void System::handleSystemEvent(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 2)
	{
		printf("woopsy\n");
	}
	else
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Local<v8::String> event = v8::Local<v8::String>::Cast(info[0]);
		v8::String::Utf8Value eventString(isolate, info[0]);
		System* self = static_cast<System*>(info.Data().As<v8::External>()->Value());
		self->registerEventListener(*eventString, v8::Local<v8::Function>::Cast(info[1]));
	}
}

void System::emitEvent(const char* event, v8::Local<v8::Value> value)
{
	const Hash hash = Hasher::hash(event);
	if (eventListeners_.contains(hash))
	{
		v8::Isolate* isolate = engine.isolate;
		v8::Local<v8::Value> recv = v8::Null(isolate);
		v8::Local<v8::Value> args[1] = { value };
		eventListeners_.at(hash).Get(isolate)->Call(isolate->GetCurrentContext(), recv, 1, args);
	}
}

void System::emitEvent(const char* event, EventDataCallback callback)
{
	const Hash hash = Hasher::hash(event);
	if (eventListeners_.contains(hash))
	{
		v8::Isolate* isolate = engine.isolate;
		v8::Local<v8::Value> recv = v8::Null(isolate);
		v8::Local<v8::Object> arg = v8::Object::New(isolate);
		ObjectBuilder builder(isolate, arg);
		callback(builder);
		v8::Local<v8::Value> args[1] = { arg };
		eventListeners_.at(hash).Get(isolate)->Call(isolate->GetCurrentContext(), recv, 1, args);
	}
}

void System::registerEventListener(const char* event, v8::Local<v8::Function> callback)
{
	const Hash hash = Hasher::hash(event);
	if (eventListeners_.contains(hash))
		eventListeners_.at(hash).Reset(engine.isolate, callback);
	else
		eventListeners_.emplace(std::make_pair(Hasher::hash(event), v8::Global<v8::Function>(engine.isolate, callback)));
}

void System::initialize(const Config& config)
{
	if (isDestroyed_)
		throw "System is destroyed!";

	if (isInitialized_)
		throw "System is already initialized!";

	onInitialize(config);
	isInitialized_ = true;
}

void System::destroy()
{
	if (isDestroyed_)
		throw "System is already destroyed!";
	if (!isInitialized_)
		throw "System is not initialized yet!";
	onDestroy();
	isDestroyed_ = true;
}
