#include "framework.hpp"
#include "Engine.hpp"

using namespace v8;

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();

	const auto setMethod = [&](Local<Object> obj, const char* name, FunctionCallback func)
	{
		obj->Set(context, String::NewFromUtf8(isolate, name).ToLocalChecked(), FunctionTemplate::New(isolate, func)->GetFunction(context).ToLocalChecked());
	};

	const auto setObject = [&](Local<Object> parent, const char* name)
	{
		auto obj = Object::New(isolate);
		parent->Set(context, String::NewFromUtf8(isolate, name).ToLocalChecked(), obj);
		return obj;
	};

	setMethod(exports, "initialize", Engine::initialize);
}