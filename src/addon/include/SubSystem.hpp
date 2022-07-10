#pragma once

#include "framework.hpp"
#include "Config.hpp"

class Engine;

class SubSystem
{
private:
	bool isInitialized_;
	
public:
	Engine& engine;

protected:
	SubSystem(Engine& engine) : isInitialized_(false), engine(engine) { }

	virtual void onInitialize(Config& config, ObjectBuilder& exports) = 0;
	virtual void onTerminate() = 0;

private:
	void initialize(Config& config, ObjectBuilder& exports)
	{
		if(!isInitialized_)
		{
			onInitialize(config, exports);
			isInitialized_ = true;
		}
	}

	void terminate() 
	{ 
		if(isInitialized_)
		{
			onTerminate();
			isInitialized_ = false;
		}
	}

	friend class Engine;
};

#define SUB_SYSTEM_CLASS(__name__) class __name__ : public SubSystem

#define SUB_SYSTEM_OVERRIDES(__name__) protected: \
	__name__ (Engine& engine) : SubSystem(engine) {} \
	void onInitialize(Config& config, ObjectBuilder& exports) override; \
	void onTerminate() override; \
	friend class Engine;