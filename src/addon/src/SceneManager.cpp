#include "SceneManager.hpp"
#include "Engine.hpp"

void SceneManager::onInitialize(Config& config, ObjectBuilder& exports)
{
	printf("SceneManager initialized!\n");
	const std::vector<std::string>& names = engine.game.getSceneNames();

	size_t i = 1;

	for (const auto& name : names)
	{
		std::string scenePath = createScenePath(i++);
		std::cout << "got scene " << name.c_str() << " with path scenes/" << scenePath.c_str() << std::endl;
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
		printf("loaded scene %s\n", scene.name().c_str());
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