#include "Entity.hpp"
#include "Engine.hpp"

void JsEntity::constructCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Engine& engine = *(static_cast<Engine*>(info.Data().As<v8::External>()->Value()));
	printf("Entity constructed!\n");

	v8::Isolate* isolate = info.GetIsolate();

	const size_t argc = info.Length();

	std::string name;
	int64_t sceneIndex;

	if (argc >= 1)
	{
		v8::String::Utf8Value utf8Val(isolate, info[0]);
		name = std::string(*utf8Val);
	}
	else
	{
		name = "Entity";
	}

	if (argc >= 2)
	{
		auto sceneObj = info[1].As<v8::Object>();
		sceneIndex = sceneObj->GetInternalField(0)->IntegerValue(isolate->GetCurrentContext()).ToChecked();
	}
	else
	{
		sceneIndex = engine.sceneManager.getActiveSceneIndex();
		if (sceneIndex < 0)
		{
			isolate->ThrowException(createString(isolate, "No scene is provided and loaded!"));
			return;
		}
	}

	Scene& scene = engine.sceneManager.getScene(sceneIndex);

	if (argc >= 3)
	{
		printf("constructed from addon\n");
	}
	else
	{
		printf("constructed from game/editor\n");
		Handle<Entity>& entityHandle = scene.addEntity(name);

		// printf("Internal field count: %d\n", info.This()->InternalFieldCount());

		info.This()->SetInternalField(0, createExternal(isolate, entityHandle));
	}
}

void JsEntity::onInit(v8::Local<v8::FunctionTemplate>& ctor)
{
	printf("js entity class init...\n");
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
}

JsEntity::JsEntity(Engine& engine) : JsClass(engine.getIsolate(), "Entity", constructCallback, std::addressof(engine))
{

}