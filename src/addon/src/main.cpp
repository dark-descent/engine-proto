#include "framework.hpp"
#include "Engine.hpp"
#include "Bin.hpp"

using namespace v8;

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();

	exports->Set(context, createString(isolate, "initialize"), createFunction(isolate, Engine::initialize));
}