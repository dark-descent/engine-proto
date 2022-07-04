#include "Engine.hpp"
#include "node.h"
#include "components/Transform.hpp"

const char* const Engine::logLevelStringMap_[] = { "error", "exception", "warn", "info" };

Engine* Engine::engine_ = nullptr;

Engine::Engine(v8::Isolate* isolate, const v8::Local<v8::Function>& logCallback) : isolate_(isolate), config(Config()), systemsStack_(), systems_(), components_(), entities_(), renderer(*this)
{
	logCallback_.Reset(isolate, logCallback);
	entityHandles_ = new EntityHandleAllocator(config.entityBlockSize);

	try
	{
		loadSystem(&renderer);
		registerComponent<Transform>();
	}
	catch (const char* msg)
	{
		log(LogLevel::EXCEPTION, msg);
	}
}

Engine::~Engine()
{

}

inline size_t Engine::getNextComponentFlag()
{
	size_t flag = componentFlagCounter_;
	componentFlagCounter_ *= 2;
	return flag;
}

EntityHandle* Engine::addEntity()
{
	size_t entityIndex = entities_.alloc();
	Entity* e = entities_.at(entityIndex);
	e->handle = entityHandles_->alloc(entityIndex);
	return e->handle;
}

inline Entity* Engine::getEntity(EntityHandle* handle)
{
	return entities_.at(handle->entityIndex);
}

void Engine::destroyEntity(EntityHandle* handle)
{
	Entity* entity = getEntity(handle);
	entities_.free(handle->entityIndex, [&](size_t oldIndex, size_t newIndex, Entity* entity) { entity->handle->entityIndex = newIndex; });
	entityHandles_->free(handle);
}

void Engine::log(LogLevel level, const char* msg)
{
	v8::Local<v8::Value> recv = v8::Object::New(isolate_);
	v8::Local<v8::Value> args[2];
	args[0] = createString(isolate_, logLevelStringMap_[level]);
	args[1] = createString(isolate_, msg);
	logCallback_.Get(isolate_)->Call(isolate_->GetCurrentContext(), recv, 2, args);
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


	v8::Local<v8::Value> val = config->ToObject(ctx).ToLocalChecked()->Get(ctx, createString(isolate, "logHandler")).ToLocalChecked();

	node::AddEnvironmentCleanupHook(isolate, Engine::cleanup, isolate);

	v8::Local<v8::Function> logCallback = v8::Local<v8::Function>::Cast(val);

	engine_ = new Engine(isolate, logCallback);

	v8::Local<v8::Object> exposeObject = createObject(isolate, [&](ObjectBuilder& builder)
	{
		builder.set("_internalPtr", createPointer(isolate, engine_));
	});

	info.GetReturnValue().Set(exposeObject);
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