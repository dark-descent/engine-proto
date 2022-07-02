#pragma once

#include "Hasher.hpp"
#include "Config.hpp"

class Engine;

#define SYSTEM_CTOR(name) private: (##name)(const Engine& engine) : System(engine) {}
#define SYSTEM_IMPL() protected: virtual void onInitialize(const Config& config) override; virtual void onDestroy() override;

class System
{
public:
	template<typename T>
	constexpr static int getTypeHash()
	{
		return Hasher::hash(typeid(T).name());
	}

private:
	bool isConfigured_ = false;
	bool isInitialized_ = false;
	bool isDestroyed_ = false;

protected:
	const Engine& engine;

	System(const Engine& engine) : engine(engine) { }

	virtual void onInitialize(const Config& config) = 0;
	virtual void onDestroy() = 0;

private:
	void initialize(const Config& config)
	{
		if (isDestroyed_)
			throw "System is destroyed!";
		
		if (isInitialized_)
			throw "System is already initialized!";
		
		onInitialize(config);
		isInitialized_ = true;
	}

	void destroy()
	{
		if (isDestroyed_)
			throw "System is already destroyed!";
		if (!isInitialized_)
			throw "System is not initialized yet!";
		onDestroy();
		isDestroyed_ = true;
	}

	friend class Engine;
};