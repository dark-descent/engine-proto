#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Config.hpp"

class Engine;

#define SYSTEM_CTOR(name) private: (##name)(const Engine& engine) : System(engine) {}
#define SYSTEM_IMPL() protected: virtual void onInitialize(const Config& config) override; virtual void onDestroy() override;

using EventDataCallback = void(ObjectBuilder&);

class System
{
public:
	template<typename T>
	constexpr static int getTypeHash()
	{
		return Hasher::hash(typeid(T).name());
	}
	static void testEvent(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void handleSystemEvent(const v8::FunctionCallbackInfo<v8::Value>&);
private:

	bool isConfigured_ = false;
	bool isInitialized_ = false;
	bool isDestroyed_ = false;

	std::unordered_map<Hash, v8::Global<v8::Function>> eventListeners_;

protected:
	const Engine& engine;

	System(const Engine& engine) : engine(engine), eventListeners_() { }

	void emitEvent(const char* event, EventDataCallback callback);
	void emitEvent(const char* event, v8::Local<v8::Value> value);
	void registerEventListener(const char* event, v8::Local<v8::Function> callback);

	virtual void onInitialize(const Config& config) = 0;
	virtual void onDestroy() = 0;

private:
	void initialize(const Config& config);
	void destroy();

	friend class Engine;
};