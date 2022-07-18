#include "Engine.hpp"
#include "components/Transform.hpp"
#include "components/RigidBody.hpp"

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
	const size_t transform = registerAndExposeComponent<Transform>(exports, "Transform");
	const size_t rigidBody = registerAndExposeComponent<RigidBody>(exports, "RigidBody");

	const auto initSubSystem = [&](SubSystem& subSystem)
	{
		subSystem.initialize(config, exports);
		subSystems_.push_back(&subSystem);
	};

	initSubSystem(assetManager);
	initSubSystem(sceneManager);

	Scene& scene = sceneManager.addScene("Test", true);

	EntityHandle& entityA = scene.addEntity();
	EntityHandle& entityB = scene.addEntity();
	EntityHandle& entityC = scene.addEntity();

	scene.addComponentToEntity(entityA, transform);
	scene.addComponentToEntity(entityB, transform);
	scene.addComponentToEntity(entityC, transform);
	scene.addComponentToEntity(entityC, rigidBody);

	Logger::log("EntityA", scene.getArchType(entityA.archType));
	Logger::log("EntityB", scene.getArchType(entityB.archType));
	Logger::log("EntityC", scene.getArchType(entityC.archType));
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