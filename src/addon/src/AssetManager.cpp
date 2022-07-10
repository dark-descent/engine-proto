#include "AssetManager.hpp"
#include "Engine.hpp"

void AssetManager::onInitialize(Config& config, ObjectBuilder& exports)
{
	printf("AssetManager initialized!\n");
	scenesPath_ = std::filesystem::path(engine.game.getGamePath()) / "scenes";
	auto s = scenesPath_.string<char>();
	printf("%s\n", s.c_str());
}

void AssetManager::onTerminate()
{

}

void AssetManager::loadSceneFile(Hash sceneName)
{
	const char* fileName = engine.game.getSceneFileName(sceneName);

	if (fileName == nullptr)
		throw std::runtime_error("scene file not found!");

	std::filesystem::path p = scenesPath_ / fileName;

	auto scenePath = p.string<char>();
 
	loadFile(scenePath.c_str(), [](const char* buffer, std::streamsize size, size_t chunkOffset)
	{
		
	});
}