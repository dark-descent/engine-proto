#pragma once

#include "framework.hpp"
#include "System.hpp"
#include "gfx/Renderer.hpp"

class Engine
{
private:
	static Engine* engine_;

public:
	static void initialize(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void destory(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void cleanup(void* data);

private:
	std::unordered_map<Hash, System*> systems_;
	std::stack<Hash> systemsStack_;
	Config config;
	gfx::Renderer renderer;

private:
	Engine();
	~Engine();

	template<typename T>
	bool loadSystem(T* system)
	{
		const Hash hash = System::getTypeHash<T>();
		system->initialize(config);
		systems_.insert(std::make_pair(hash, system));
		systemsStack_.push(hash);
		return true;
	}
};