#include "Engine.hpp"
#include "components/Transform.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"

Engine* Engine::engine_ = nullptr;

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
			Logger::get("Test").error("Exception: ", e.what());
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
	logger(Logger::get("internal")),
	game()
{
	printf("init :D \n");
	registerAndExposeComponent<Transform>(exports, "Transform");
	registerAndExposeComponent<RigidBody>(exports, "RigidBody");
	registerAndExposeComponent<BoxCollider>(exports, "BoxCollider");
	registerAndExposeComponent<CircleCollider>(exports, "CircleCollider");

	const auto initSubSystem = [&](SubSystem& subSystem)
	{
		subSystem.initialize(config, exports);
		subSystems_.push_back(&subSystem);
	};

	initSubSystem(assetManager);
	initSubSystem(sceneManager);

	Scene& scene = sceneManager.addScene("Test", true);

	// for (size_t i = 0; i < 100; i++)
	// {
	// 	auto& entityA = scene.addEntity();
	// 	scene.addComponentToEntity(entityA, 0);
	// }

	// ArchType& arch = scene.getArchType(1);

	// Logger& logger = Logger::get("Test");
	// logger.info("Helloa :D");

}

Engine::~Engine()
{

}

const size_t Engine::getComponentSize(size_t index)
{
	return components_.at(index).getSize();
}

const size_t Engine::getComponentBitMask(size_t index)
{
	return components_.at(index).getBitMask();
}

const Component& Engine::getComponent(size_t index)
{
	return components_.at(index);
}