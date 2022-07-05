#include "Engine.hpp"
#include "node.h"
#include "components/Transform.hpp"
#include <chrono>

const char* const Engine::logLevelStringMap_[] = { "error", "exception", "warn", "info" };

Engine* Engine::engine_ = nullptr;

Engine::Engine(v8::Isolate* isolate, const v8::Local<v8::Function>& logCallback) : isolate_(isolate), config(Config()), systemsStack_(), systems_(), components_(), componentTypes_(), entities_(sizeof(Entity), 1024, Engine::onEntityRellocateCallback, this), renderer(*this)
{
	logCallback_.Reset(isolate, logCallback);
	entityHandles_ = new EntityHandleAllocator(config.entityBlockSize);

	try
	{
		loadSystem(&renderer);
		registerComponent<Transform>();

		const size_t l = 2048;

		for (size_t i = 0; i < l; i++)
		{
			EntityHandle* handle = addEntity();
			Transform* t = addEntityComponent<Transform>(handle);
			t->position.x = i;
			t->position.y = i;
		}

		for (size_t i = 0; i < l; i++)
		{
			if (i < 2028 || (i % 2 == 0))
			{
				destroyEntity(entityHandles_->at(i));
			}
		}

		entityHandles_->iterate([&](EntityHandle* handle, size_t i)
		{
			Entity* entity = getEntity(handle);
			Transform* t = static_cast<Transform*>(entity->firstComponent);
			printf("got entity at index: %zu with transform [x: %f, y: %f]\n", handle->entityIndex, t->position.x, t->position.y);
		});

		printf("end!");
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
	Entity* e = reinterpret_cast<Entity*>(entities_.at(entityIndex));
	memset(e, 0, sizeof(Entity));
	e->handle = entityHandles_->alloc(entityIndex);
	return e->handle;
}

inline Entity* Engine::getEntity(EntityHandle* handle)
{
	return static_cast<Entity*>(entities_.at(handle->entityIndex));
}

void Engine::destroyEntity(EntityHandle* handle)
{
	Entity* entity = getEntity(handle);

	Component* c = entity->firstComponent;

	entities_.free(handle->entityIndex);

	while (c != nullptr)
	{
		printf("destroy component!\n");
		AlignedAllocator* allocator = static_cast<AlignedAllocator*>(c->componentList);
		allocator->free(c->componentIndex);
		c = c->nextComponent;
	}

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

void Engine::onComponentRellocateCallback(size_t newIndex, void* data, void* engine)
{
	Component* component = static_cast<Component*>(data);

	printf("component rellocate %zu -> %zu\n", component->componentIndex, newIndex);

	component->componentIndex = newIndex;

	if (component->prevComponent != nullptr)
	{
		// printf("not first component!\r\n");
		component->prevComponent->nextComponent = component;
	}
	else
	{
		// printf("first component!\r\n");
		// if(component->entity == nullptr)
		// {
		// 	printf("entity is nullptr!\r\n");
		// }
		// else
		// {
		component->entity->firstComponent = component;
		// printf("setted entity firstComponent!\r\n");
	// }
	}

	if (component->nextComponent != nullptr)
	{
		// printf("not last component!\r\n");
		component->nextComponent->prevComponent = component;
	}
	else
	{
		// printf("last component!\r\n");
		// if(component->entity == nullptr)
		// {
		// 	printf("entity is nullptr!\r\n");
		// }
		// else
		// {
		component->entity->lastComponent = component;
		// 	printf("setted entity lastComponent!\r\n");
		// }
	}
}

void Engine::onEntityRellocateCallback(size_t newIndex, void* data, void* engine)
{
	Entity* entity = static_cast<Entity*>(data);
	printf("Entity rellocate %zu -> %zu\n", entity->handle->entityIndex, newIndex);
	if (entity->handle != nullptr)
	{
		entity->handle->entityIndex = newIndex;
		Component* c = entity->firstComponent;
		while (c != nullptr)
		{
			c->entity = entity;
			c = c->nextComponent;
		}
	}
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