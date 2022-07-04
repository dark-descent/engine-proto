#pragma once

#include "framework.hpp"
#include "System.hpp"
#include "Component.hpp"
#include "Entity.hpp"
#include "gfx/Renderer.hpp"
#include "allocators/AlignedAllocator.hpp"
#include "allocators/EntityHandleAllocator.hpp"
#include "Config.hpp"

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

public:
	static void initialize(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void destory(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void cleanup(void* data);

private:
	v8::Isolate* isolate_;
	std::unordered_map<Hash, System*> systems_;
	std::stack<Hash> systemsStack_;
	std::unordered_map<Hash, void*> components_;
	size_t componentFlagCounter_ = 1;
	std::unordered_map<Hash, int> componentFlags_;
	AlignedAllocator<Entity> entities_;
	EntityHandleAllocator* entityHandles_;
	Config config;
	gfx::Renderer renderer;

	v8::Persistent<v8::Function> logCallback_;


	Engine(v8::Isolate* isolate, const v8::Local<v8::Function>& logCallback);
	~Engine();

	inline size_t getNextComponentFlag();
	
	EntityHandle* addEntity();
	inline Entity* getEntity(EntityHandle* handle);
	void destroyEntity(EntityHandle* handle);

	void log(LogLevel level, const char* msg);

	template<typename T>
	bool loadSystem(T* system)
	{
		const Hash hash = System::getTypeHash<T>();
		system->initialize(config);
		systems_.insert(std::make_pair(hash, system));
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
			void* allocator = static_cast<void*>(new AlignedAllocator<T>());
			components_.insert(std::make_pair(hash, allocator));
			size_t flag = getNextComponentFlag();
			componentFlags_.insert(std::make_pair(hash, flag));

			char buff[100];
			snprintf(buff, sizeof(buff), "Registered component %s with bit flag %zu", typeid(T).name(), flag);
			log(LogLevel::INFO, buff);
		}
	}
};