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
private:
	static v8::Persistent<v8::Symbol> internalSymbol_;

	v8::FunctionCallback ctorCallback_;

	static void ctorWrapper(V8CallbackArgs args);
	static void getIndexCallback(V8CallbackArgs args);

	uint64_t index_;

protected:
	Engine& engine;

public:
	static v8::Local<v8::Symbol> getInternalSymbol(v8::Isolate* isolate);
	JsComponent(Engine& engine, const char* name, uint64_t index, v8::FunctionCallback callback);
};

#define JS_COMPONENT_CTOR(__NAME__, __JS_CTOR__) __NAME__(Engine& engine, const char* name, uint64_t index) : JsComponent(engine, name, index, __JS_CTOR__)