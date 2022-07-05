#pragma once

#include "framework.hpp"
#include "System.hpp" 
#include "Component.hpp"
#include "Entity.hpp"
#include "gfx/Renderer.hpp"
#include "allocators/AlignedAllocator.hpp"
#include "allocators/EntityHandleAllocator.hpp"
#include "Config.hpp"
#include "ComponentFamily.hpp"

enum LogLevel
{
	ERROR = 0,
	EXCEPTION = 1,
	WARN = 2,
	INFO = 3,
	SIZE
};

class Engine
{
private:
	static const char* const logLevelStringMap_[LogLevel::SIZE];

	static Engine* engine_;

	static void onComponentRellocateCallback(size_t newIndex, void* data, void* engine);

	static void onEntityRellocateCallback(size_t newIndex, void* data, void* engine);

public:
	static void initialize(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void destory(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void cleanup(void* data);


	v8::Isolate* isolate;
private:
	std::unordered_map<Hash, System*> systems_;
	std::unordered_map<Hash, const char*> systemsNames_;
	std::stack<Hash> systemsStack_;
	std::unordered_map<Hash, AlignedAllocator*> components_;
	size_t componentFlagCounter_ = 1;
	std::unordered_map<Hash, int> componentTypes_;
	AlignedAllocator entities_;
	EntityHandleAllocator* entityHandles_;
	Config config;
	gfx::Renderer renderer;

	v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> logCallback_;

	Engine(v8::Isolate* isolate, const v8::Local<v8::Function>& logCallback);
	~Engine();

	void exposeInternals(ObjectBuilder& builder);

	inline size_t getNextComponentFlag();

	EntityHandle* addEntity();
	inline Entity* getEntity(EntityHandle* handle);
	void destroyEntity(EntityHandle* handle);

	void log(LogLevel level, const char* msg);

	template<typename T>
	T* addEntityComponent(Entity* entity)
	{
		if (entity != nullptr)
		{
			const Hash hash = System::getTypeHash<T>();

			if (components_.contains(hash))
			{
				entity->family |= componentTypes_.at(hash);

				AlignedAllocator* allocator = getComponentAllocator(hash);

				if (allocator == nullptr)
					return nullptr;

				size_t index = allocator->alloc();

				T* component = static_cast<T*>(allocator->at(index));

				if (component != nullptr)
				{
					memset(component, 0, sizeof(T));
					component->componentIndex = index;
					component->componentList = static_cast<void*>(allocator);

					if (entity->firstComponent == nullptr)
					{
						entity->firstComponent = component;
						entity->lastComponent = component;
					}
					else
					{
						entity->lastComponent->nextComponent = component;
						component->prevComponent = entity->lastComponent;
						entity->lastComponent = component;
					}
				}

				return component;
			}
		}
		return nullptr;
	}

	template<typename T>
	T* addEntityComponent(EntityHandle* handle)
	{
		return addEntityComponent<T>(getEntity(handle));
	}

	template<typename T>
	bool registerSystem(const char* systemExposeName, T* system)
	{
		const Hash hash = System::getTypeHash<T>();
		system->initialize(config);
		systems_.insert(std::make_pair(hash, system));
		systemsNames_.insert(std::make_pair(hash, systemExposeName));
		systemsStack_.push(hash);
		return true;
	}

	template<typename T, typename std::enable_if<std::is_base_of<Component, T>::value>::type* = nullptr>
	void registerComponent()
	{
		const Hash hash = System::getTypeHash<T>();
		if (components_.contains(hash))
		{
			char buff[100];
			snprintf(buff, sizeof(buff), "Component %s is already registered!", typeid(T).name());
			log(LogLevel::WARN, buff);
		}
		else
		{
			AlignedAllocator* allocator = new AlignedAllocator(sizeof(T), 1024, Engine::onComponentRellocateCallback, this);
			components_.insert(std::make_pair(hash, allocator));
			size_t flag = getNextComponentFlag();
			componentTypes_.insert(std::make_pair(hash, flag));

			char buff[100];
			snprintf(buff, sizeof(buff), "Registered component %s with bit flag %zu", typeid(T).name(), flag);
			log(LogLevel::INFO, buff);
		}
	}

	template<typename T>
	AlignedAllocator* getComponentAllocator()
	{
		const Hash hash = System::getTypeHash<T>();
		return getComponentAllocator(hash);
	}

	AlignedAllocator* getComponentAllocator(const Hash hash)
	{
		if (components_.contains(hash))
			return components_.at(hash);
		return nullptr;
	}
};