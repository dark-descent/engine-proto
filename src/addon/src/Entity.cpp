#include "Entity.hpp"
#include "Engine.hpp"

void JsEntity::constructCallback(V8CallbackArgs args)
{
	Engine* engine = getExternalData<Engine*>(args);

	v8::Isolate* isolate = args.GetIsolate();

	const size_t argc = args.Length();

	std::string name;
	int64_t sceneIndex;

	if (argc >= 1)
	{
		v8::String::Utf8Value utf8Val(isolate, args[0]);
		name = std::string(*utf8Val);
	}
	else
	{
		name = "Entity";
	}

	if (argc >= 2)
	{
		auto sceneObj = args[1].As<v8::Object>();
		sceneIndex = sceneObj->GetInternalField(0)->IntegerValue(isolate->GetCurrentContext()).ToChecked();
	}
	else
	{
		sceneIndex = engine->sceneManager.getActiveSceneIndex();
		if (sceneIndex < 0)
		{
			engine->logger.error("Exception: No scene is provided and loaded!");
			isolate->ThrowException(createString(isolate, "No scene is provided and loaded!"));
			return;
		}
	}

	Scene& scene = engine->sceneManager.getScene(sceneIndex);

	if (argc >= 3)
	{

	}
	else
	{
		Handle<Entity>& entityHandle = scene.addEntity(name);
		args.This()->SetInternalField(0, createExternal(isolate, entityHandle));
	}
}

void JsEntity::addComponent(V8CallbackArgs args)
{
	V8UnpackArgs(args);

	auto engine = getExternalData<Engine*>(args);

	if (argc == 0)
	{
		engine->logger.error("Exception: No scene is provided and loaded!");
		isolate->ThrowException(createString(isolate, "No component type provided!"));
		return;
	}

	bool invalidCall = false;

	if (args[0]->IsFunction())
	{
		auto ctor = args[0].As<v8::Function>();
		auto maybeIndex = ctor->Get(isolate->GetCurrentContext(), createString(isolate, "index"));
		if (!maybeIndex.IsEmpty())
		{

			const uint64_t index = maybeIndex.ToLocalChecked()->IntegerValue(context).ToChecked();
			const uint64_t bitMask = engine->getComponent(index).getBitMask();
			engine->logger.info("add component with index ", index);

			Handle<Entity>* entity = getExternalData<Handle<Entity>*>(args.This()->GetInternalField(0));

			void* componentData = nullptr;

			if ((entity->data.arch->data.bitMask & bitMask) == bitMask)
			{
				engine->logger.warn("entity already has component with index ", index);
				componentData = entity->data.getComponent(index);
			}
			else
			{
				componentData = engine->sceneManager.getActiveScene().addComponentToEntity(*entity, index, nullptr);
			}

			v8::Local<v8::Value> componentArgs[3] = {};
			componentArgs[0] = JsComponent::getInternalSymbol(isolate);
			componentArgs[1] = args.This();
			componentArgs[2] = createExternal(isolate, componentData);

			auto component = ctor->NewInstance(context, 3, componentArgs).ToLocalChecked();
			args.GetReturnValue().Set(component);
			return;
		}
	}

	engine->logger.error("Exception: Invalid component type!");
	isolate->ThrowException(createString(isolate, "Invalid component type!"));
	return;
}

void JsEntity::getComponent(V8CallbackArgs args)
{

}

void JsEntity::removeComponent(V8CallbackArgs args)
{

}


JsEntity::JsEntity(Engine& engine) : JsClass(engine.getIsolate(), "Entity", constructCallback, std::addressof(engine))
{
	v8::Isolate* isolate = engine.getIsolate();
	auto ctor = template_.Get(isolate);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->PrototypeTemplate()->Set(isolate, "name", createString(isolate, "Entity"));
	ctor->PrototypeTemplate()->Set(isolate, "addComponent", v8::FunctionTemplate::New(isolate, addComponent, createExternal(isolate, engine)));
	ctor->PrototypeTemplate()->Set(isolate, "getComponent", v8::FunctionTemplate::New(isolate, getComponent, createExternal(isolate, engine)));
	ctor->PrototypeTemplate()->Set(isolate, "removeComponent", v8::FunctionTemplate::New(isolate, removeComponent, createExternal(isolate, engine)));
}