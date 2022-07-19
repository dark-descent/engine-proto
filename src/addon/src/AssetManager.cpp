#include "AssetManager.hpp"
#include "Engine.hpp"

void AssetManager::onInitialize(Config& config, ObjectBuilder& exports)
{
	engine.logger.info("AssetManager initialized!");
}

void AssetManager::onTerminate()
{

}