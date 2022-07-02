#include "framework.hpp"
#include "Engine.hpp"

using namespace v8;

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();

	const auto setMethod = [&](v8::Local<v8::Object> obj, const char* name, v8::FunctionCallback func) {
		obj->Set(context, String::NewFromUtf8(isolate, name).ToLocalChecked(), FunctionTemplate::New(isolate, func)->GetFunction(context).ToLocalChecked()).FromJust();
	};

	const auto setObject = [&](const char* name)
	{

	};

	setMethod(exports, "initialize", Engine::initialize);
}