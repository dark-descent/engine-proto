#include "Engine.hpp"
#include "components/Transform.hpp"
#include "components/RigidBody.hpp"
#include "SimpleBinary.hpp"

Engine* Engine::engine_ = nullptr;
uint8_t Engine::componentBitMaskCounter_ = 1;

void Engine::initialize(V8CallbackArgs args)
{
	using namespace v8;

	Isolate* isolate = args.GetIsolate();

	if (engine_ != nullptr)
	{
		isolate->ThrowException(createString(isolate, "Engine is already initialized!"));
	}
	else
	{
		try
		{
			if (args.Length() != 1)
			{
				throwException(isolate, "Invalid number of arguments!");
				return;
			}
			else if (!args[0]->IsObject())
			{
				throwException(isolate, "argument at index 0 is not of type \"Object\"!");
				return;
			}

			ObjectBuilder exports(isolate);
			Config config(v8::Local<v8::Object>::Cast(args[0]));

			node::AddEnvironmentCleanupHook(isolate, Engine::destroy, isolate);

			engine_ = new Engine(config, exports);

			args.GetReturnValue().Set(exports.build());
		}
		catch (std::runtime_error e)
		{
			printf("Exception: %s\n", e.what());
			isolate->ThrowException(createString(isolate, e.what()));
		}
	}
}

void Engine::destroy(void* data)
{
	delete Engine::engine_;
	Engine::engine_ = nullptr;
	v8::Isolate* isolate = static_cast<v8::Isolate*>(data);
	node::RemoveEnvironmentCleanupHook(isolate, Engine::destroy, isolate);
}

Engine::Engine(Config& config, ObjectBuilder& exports) :
	sceneManager(*this),
	assetManager(*this),
	subSystems_(),
	game()
{
	registerAndExposeComponent<Transform>(exports, "Transform");
	registerAndExposeComponent<RigidBody>(exports, "RigidBody");

	const auto initSubSystem = [&](SubSystem& subSystem)
	{
		subSystem.initialize(config, exports);
		subSystems_.push_back(&subSystem);
	};

	initSubSystem(assetManager);
	initSubSystem(sceneManager);

	assetManager.loadSceneFile(Hasher::hash("First Scene"));

	{
		SimpleBinary::Writer writer("H:\\dmtrllv\\Code\\dark-descent\\editor\\test.bin");

		writer.write(true);
		writer.write(false);
		writer.write(0.123f);
		writer.writeStr("hello world");
		writer.write(9.876f);
		writer.write(false);
		writer.write(true);
	}

	{
		SimpleBinary::Reader reader("H:\\dmtrllv\\Code\\dark-descent\\editor\\test.bin");

		reader.read([](SimpleBinary::Types type, void* value, size_t index)
		{
			switch (type)
			{
			case SimpleBinary::Types::BOOL:
				printf("got boolean: %s\n", SimpleBinary::cast<bool>(value) ? "true" : "false");
				break;
			case SimpleBinary::Types::FLOAT:
				printf("got float: %f\n", SimpleBinary::cast<float>(value));
				break;
			case SimpleBinary::Types::STRING:
				printf("got string: %s\n", static_cast<char*>(value));
			}
		});
	}
}

Engine::~Engine()
{

}