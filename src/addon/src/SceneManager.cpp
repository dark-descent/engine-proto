#include "SceneManager.hpp"
#include "Engine.hpp"

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
}

void SceneManager::onTerminate()
{

}

Scene& SceneManager::loadScene(const Hash hash)
{
	if (!scenesIndices_.contains(hash))
		throw std::runtime_error("Could not load scene!");

	Scene& scene = getScene(hash);

	std::filesystem::path scenesPath = std::filesystem::path(engine.game.getGameDir()) / "scenes" / scene.path();

	Bin::Reader reader(scenesPath.string());

	reader.read([&](Bin::Parser& parser)
	{
		activeScene_ = std::addressof(scene);
	});

	return *activeScene_;
}

Scene& SceneManager::addScene(std::string name, bool load)
{
	return addScene(name, createScenePath(scenes_.size() + 1), load);
}

Scene& SceneManager::addScene(std::string name, std::string path, bool load)
{
	Hash h = Hasher::hash(name.c_str());
	
	if (scenesIndices_.contains(h))
		throw std::runtime_error("Scene already exists!");
	
	size_t newSize = scenes_.size();
	
	if ((newSize + 1) % sceneBufferSize == 0)
	{
		engine.logger.info("Increase scenes buffer");
		scenesIndices_.reserve(sceneBufferSize);
		scenes_.reserve(sceneBufferSize);
	}

	scenesIndices_.emplace(h, newSize);
	
	Scene& scene = scenes_.emplace_back(engine, name, path);
	
	if (load)
		activeScene_ = std::addressof(scene);
	
	return *activeScene_;
}

Scene& SceneManager::getActiveScene()
{
	if (activeScene_ == nullptr)
		throw std::runtime_error("No scenes are loaded yet!");

	return *activeScene_;
}


Scene& SceneManager::getScene(const Hash hash)
{
	if (scenesIndices_.contains(hash))
		return scenes_.at(scenesIndices_.at(hash));

	throw std::runtime_error("Could not get scene!");
}