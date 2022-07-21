#include "Engine.hpp"
#include "components/Transform.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "System.hpp"

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
	componentTypeMap_(),
	assetManager(*this),
	sceneManager(*this),
	renderer(*this),
	subSystems_(),
	logger(Logger::get("internal")),
	game()
{
	printf("init :D \n");
	auto transform = registerAndExposeComponent<Transform>(exports, "Transform");
	auto rigidBody = registerAndExposeComponent<RigidBody>(exports, "RigidBody");
	auto boxCollider = registerAndExposeComponent<BoxCollider>(exports, "BoxCollider");
	auto circleCollider = registerAndExposeComponent<CircleCollider>(exports, "CircleCollider");

	const auto initSubSystem = [&](SubSystem& subSystem)
	{

		subSystem.initialize(config, exports);
		subSystems_.push_back(&subSystem);
	};

	initSubSystem(assetManager);
	initSubSystem(sceneManager);
	initSubSystem(renderer);

	Scene& scene = sceneManager.addScene("Test", true);

	auto& entityA = scene.addEntity();
	auto& entityB = scene.addEntity();
	auto& entityC = scene.addEntity();
	auto& entityD = scene.addEntity();

	auto pl = []() { printf("------------------------------------------------\n"); };

	pl();

	Transform t;
	t.position.x = 1.0f;
	t.position.y = 1.0f;
	t.rotation.x = 1.0f;
	t.rotation.y = 1.0f;
	t.scale.x = 1.0f;
	t.scale.y = 1.0f;

	RigidBody rb;
	rb.mass = 123.0f;
	rb.velocity.x = 0.0f;
	rb.velocity.y = -0.98f;

	BoxCollider bc;
	bc.scale.x = 1.0f;
	bc.scale.y = 1.0f;

	scene.addComponentToEntity(entityA, transform, &t);
	scene.addComponentToEntity(entityA, rigidBody, &rb);
	scene.addComponentToEntity(entityA, boxCollider, &bc);

	pl();
	rb.mass = 1337.070819f;

	scene.addComponentToEntity(entityB, boxCollider, &bc);
	scene.addComponentToEntity(entityB, transform, &t);
	scene.addComponentToEntity(entityB, rigidBody, &rb);

	pl();

	RigidBody* rbA = entityA->getComponent<RigidBody>(rigidBody);
	if (rbA != nullptr)
		logger.info("got rigidbody A mass: ", rbA->mass);

	RigidBody* rbB = entityB->getComponent<RigidBody>(rigidBody);
	if (rbB != nullptr)
		logger.info("got rigidbody B mass: ", rbB->mass);

	renderer.render();
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

const ComponentInfo& Engine::getComponent(size_t index)
{
	return components_.at(index);
}

void Engine::runSystem(System& system)
{
	auto info = system.query();
	Scene& scene = sceneManager.getActiveScene();
	ArchGroup archTypes;
	size_t count = scene.filterArchTypes(info.required, archTypes);
	system.run(archTypes, count);
}