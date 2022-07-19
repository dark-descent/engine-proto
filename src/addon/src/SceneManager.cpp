#include "SceneManager.hpp"
#include "Engine.hpp"

void SceneManager::onInitialize(Config& config, ObjectBuilder& exports)
{
	engine.logger.info("SceneManager initialized!");
	const std::vector<std::string>& names = engine.game.getSceneNames();

	size_t i = 1;

	for (const auto& name : names)
	{
		std::string scenePath = createScenePath(i++);
		engine.logger.info("Found scene ", name, " with path scenes/", scenePath);
		const Hash h = Hasher::hash(name.c_str());
		scenes_.emplace(h, Scene(engine, name, scenePath));
	}
}

void SceneManager::onTerminate()
{

}

Scene& SceneManager::loadScene(const Hash hash)
{
	if (!scenes_.contains(hash))
		throw std::runtime_error("Could not load scene!");

	Scene& scene = scenes_.at(hash);

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
	Hash h = Hasher::hash(name.c_str());
	if (scenes_.contains(h))
		throw std::runtime_error("Scene already exists!");
	scenes_.emplace(std::make_pair(h, Scene(engine, name, createScenePath(scenes_.size() + 1))));
	if (load)
		activeScene_ = std::addressof(scenes_.at(h));
	return *activeScene_;
}

Scene& SceneManager::getActiveScene()
{
	if(activeScene_ == nullptr)
		throw std::runtime_error("No scenes are loaded yet!");

	return *activeScene_;
}