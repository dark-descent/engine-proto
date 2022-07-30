#include "SceneManager.hpp"
#include "Engine.hpp"
#include "exception_wrapper.hpp"

void SceneManager::addSceneCallback(V8CallbackArgs args)
{
	exceptionWrapper(args, [](V8CallbackArgs args)
	{
		SceneManager* sm = getExternalData<SceneManager*>(args);

		std::string sceneName;

		if (args.Length() > 0)
		{
			v8::String::Utf8Value utf8Val(args.GetIsolate(), args[0]);
			sceneName = std::string(*utf8Val);
		}

		args.GetReturnValue().Set(createNumber(args.GetIsolate(), sm->addScene(sceneName, true)));
	});
}

void SceneManager::addEntityCallback(V8CallbackArgs args)
{
	exceptionWrapper(args, [](V8CallbackArgs args)
	{
		SceneManager* sm = getExternalData<SceneManager*>(args);

		std::string entityName;

		size_t sceneIndex;

		if (args.Length() > 0)
		{
			v8::String::Utf8Value utf8Val(args.GetIsolate(), args[0]);
			entityName = std::string(*utf8Val);
		}

		if (args.Length() == 2)
		{
			sceneIndex = args[0]->Int32Value(args.GetIsolate()->GetCurrentContext()).ToChecked();
		}
		else
		{
			sceneIndex = sm->activeScene_;
		}

		sm->engine.logger.info("Add entity with name ", entityName, " to scene at index: ", sceneIndex);

		Scene& s = sm->scenes_.at(sceneIndex);
		Handle<Entity>& entity = s.addEntity();
		auto index = HandleIndex::serialize(entity.index);

		args.GetReturnValue().Set(createNumber(args.GetIsolate(), index));
	});

}

void SceneManager::onInitialize(Config& config, ObjectBuilder& exports)
{
	scenesIndices_.reserve(sceneBufferSize);
	scenes_.reserve(sceneBufferSize);

	engine.logger.info("SceneManager initialized!");
	const std::vector<std::string>& names = engine.game.getSceneNames();

	size_t i = 1;

	for (const auto& name : names)
	{
		std::string scenePath = createScenePath(i++);
		engine.logger.info("Found scene ", name, " with path scenes/", scenePath);
		addScene(name, scenePath);
	}

	exports.setFunction<SceneManager>("addScene", SceneManager::addSceneCallback, this);
	exports.setFunction<SceneManager>("addEntity", SceneManager::addEntityCallback, this);
	// exports.setFunction<SceneManager>("getScene", SceneManager::getSceneCallback, this);
}

void SceneManager::onTerminate()
{

}

size_t SceneManager::loadScene(const Hash hash)
{
	if (!scenesIndices_.contains(hash))
		throw std::runtime_error("Could not load scene!");

	const size_t index = getSceneIndex(hash);
	Scene& scene = getScene(index);

	std::filesystem::path scenesPath = std::filesystem::path(engine.game.getGameDir()) / "scenes" / scene.path();

	Bin::Reader reader(scenesPath.string());

	reader.read([&](Bin::Parser& parser)
	{
		activeScene_ = index;
	});

	return index;
}

size_t SceneManager::addScene(std::string name, bool load)
{
	return addScene(name, createScenePath(scenes_.size() + 1), load);
}

size_t SceneManager::addScene(std::string name, std::string path, bool load)
{
	Hash h = Hasher::hash(name.c_str());

	if (scenesIndices_.contains(h))
		throw std::runtime_error("Scene already exists!");

	size_t index = scenes_.size();

	if ((index + 1) % sceneBufferSize == 0)
	{
		engine.logger.info("Increase scenes buffer");
		scenesIndices_.reserve(sceneBufferSize);
		scenes_.reserve(sceneBufferSize);
	}

	scenesIndices_.emplace(h, index);

	Scene& scene = scenes_.emplace_back(engine, name, path);

	if (load)
		activeScene_ = index;

	return index;
}

Scene& SceneManager::getActiveScene()
{
	if (!isLoaded_)
		throw std::runtime_error("No scenes are loaded yet!");
	engine.logger.info("Get Active Scene");
	engine.logger.info("scenes_.size() => ", scenes_.size());
	return scenes_.at(activeScene_);
}


size_t SceneManager::getSceneIndex(const Hash hash)
{
	if (scenesIndices_.contains(hash))
		return scenesIndices_.at(hash);

	throw std::runtime_error("Could not get scene!");
}

Scene& SceneManager::getSceneFromHash(const Hash hash)
{
	if (scenesIndices_.contains(hash))
		return scenes_.at(scenesIndices_.at(hash));

	throw std::runtime_error("Could not get scene!");
}

Scene& SceneManager::getScene(const size_t index)
{
	return scenes_.at(index);
}