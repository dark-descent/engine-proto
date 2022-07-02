#include <node.h>

using namespace v8;

class AddonData
{
public:
	int call_count;
	
	explicit AddonData(Isolate* isolate) : call_count(0)
	{
		node::AddEnvironmentCleanupHook(isolate, DeleteInstance, this);
	}

	static void DeleteInstance(void* data)
	{
		delete static_cast<AddonData*>(data);
	}
};

static void Method(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	AddonData* data = reinterpret_cast<AddonData*>(info.Data().As<External>()->Value());
	data->call_count++;
	info.GetReturnValue().Set((double)data->call_count);
}

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();
	AddonData* data = new AddonData(isolate);
	Local<External> external = External::New(isolate, data);
	exports->Set(context, String::NewFromUtf8(isolate, "method").ToLocalChecked(), FunctionTemplate::New(isolate, Method, external)->GetFunction(context).ToLocalChecked()).FromJust();
}