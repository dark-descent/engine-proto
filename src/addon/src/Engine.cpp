#include "Engine.hpp"
#include "components/Transform.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "System.hpp"

Engine* Engine::engine_ = nullptr;

Engine* Engine::get()
{
	if (engine_ == nullptr)
		throw std::runtime_error("Engine is not initialized!");
	return engine_;
}

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
			puts("create new engine\n");
			engine_ = new Engine(isolate, config, exports);

			args.GetReturnValue().Set(exports.build());

		}
		catch (std::runtime_error e)
		{
			Logger::get("Test").error("Exception: ", e.what());
			isolate->ThrowException(createString(isolate, e.what()));
		}
	}
}

void Engine::initializeWorker(V8CallbackArgs args)
{
	using namespace v8;

	Isolate* isolate = args.GetIsolate();

	if (engine_ == nullptr)
	{
		isolate->ThrowException(createString(isolate, "Engine is not initialized yet!"));
	}
	else
	{
		try
		{
			ObjectBuilder exports(isolate);
			// node::AddEnvironmentCleanupHook(isolate, Engine::destroy, isolate);

			// engine_ = new Engine(config, exports);
			args.GetReturnValue().SetUndefined();
		}
		catch (std::runtime_error e)
		{
			// Logger::get("Test").error("Exception: ", e.what());
			// isolate->ThrowException(createString(isolate, e.what()));
		}
	}
}

void Engine::destroy(void* data)
{
	puts("destroy engine\n");
	delete Engine::engine_;
	Engine::engine_ = nullptr;
	v8::Isolate* isolate = static_cast<v8::Isolate*>(data);
	node::RemoveEnvironmentCleanupHook(isolate, Engine::destroy, isolate);
}

v8::Isolate* Engine::getIsolate()
{
	return isolate_;
}

v8::Local<v8::Context> Engine::getContext()
{
	return isolate_->GetCurrentContext();
}

Engine::Engine(v8::Isolate* isolate, Config& config, ObjectBuilder& exports) :
	components_(),
	jsComponents_(),
	componentTypeMap_(),
	assetManager(*this),
	sceneManager(*this),
	renderer(*this),
	subSystems_(),
	isolate_(isolate),
	jsEntity(*this),
	logger(Logger::get("internal")),
	game()
{
	jsEntity.init(isolate);

	exports.setArray("components", [&](ArrayBuilder& builder)
	{
		registerAndExposeComponent<Transform, JsTransform>(builder, "Transform");
		registerAndExposeComponent<RigidBody, JsRigidBody>(builder, "RigidBody");
		registerAndExposeComponent<BoxCollider, JsBoxCollider>(builder, "BoxCollider");
		registerAndExposeComponent<CircleCollider, JsCircleCollider>(builder, "CircleCollider");
	});

	exports.setVal("Entity", jsEntity.getClass(isolate));

	// jsComponents_.at(1)->create(isolate_)

	const auto initSubSystem = [&](SubSystem& subSystem)
	{
		subSystem.initialize(config, exports);
		subSystems_.push_back(&subSystem);
	};

	initSubSystem(assetManager);
	initSubSystem(sceneManager);
	initSubSystem(renderer);
}

Engine::~Engine()
{
	logger.terminate();
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