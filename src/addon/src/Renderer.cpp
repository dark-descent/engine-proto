#include "Renderer.hpp"

#include "Engine.hpp"

void Renderer::onInitialize(Config& config, ObjectBuilder& exports)
{
	auto isolate = engine.getIsolate();
	auto ctx = engine.getContext();

	auto configObj = config.jsConfig.Get(isolate)->ToObject(ctx).ToLocalChecked();
	// auto gpuObj = configObj->Get(ctx, createString(isolate, "gpu")).ToLocalChecked().As<v8::Object>();

	// auto requestAdapterFunc = gpuObj->Get(ctx, createString(isolate, "requestAdapter")).ToLocalChecked().As<v8::Function>();

	// v8::Local<v8::Value> argv[1] = {};

	// argv[0] = createObject(isolate, [](ObjectBuilder& builder)
	// {
	// 	builder.set("powerPreference", "high-performance");
	// });

	// engine.logger.info("call gpu request adapter...");

	// requestAdapterFunc->CallAsFunction(ctx, gpuObj, 1, argv);

	// engine.logger.info("call gpu request adapter oki :D");

	// exports.setFunction("resetCanvas", resetCanvas, this);
	
	renderSystem.initialize(engine);
}

void Renderer::onTerminate()
{

}

void Renderer::render()
{
	engine.runSystem(renderSystem);
}