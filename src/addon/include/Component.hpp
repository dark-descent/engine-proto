#pragma once

#include "framework.hpp"

using ComponentIndex = uint64_t;

class ComponentInfo
{
	ComponentIndex index_;
	size_t bitMask_;
	size_t size_;

public:
	ComponentInfo(ComponentIndex index, size_t bitMask, size_t size);

	const ComponentIndex getIndex() const;
	const size_t getSize() const;
	const size_t getBitMask() const;
};

class Engine;

class JsComponent : public JsClass
{
protected:
	static inline Engine& fetchEngineFromInfo(const v8::FunctionCallbackInfo<v8::Value>& info) { return *(static_cast<Engine*>(info.Data().As<v8::External>()->Value())); }

	Engine& engine;

public:
	JsComponent(Engine& engine, const char* name, v8::FunctionCallback callback);
};

#define JS_COMPONENT_CTOR(__NAME__, __JS_CTOR__) __NAME__(Engine& engine, const char* name) : JsComponent(engine, name, __JS_CTOR__)
#define FETCH_ENGINE()