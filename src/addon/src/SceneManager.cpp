#include "SceneManager.hpp"
#include "Engine.hpp"


void SceneManager::onInitialize(Config& config, ObjectBuilder& exports)
{
	printf("SceneManager initialized!\n");
}

void SceneManager::onTerminate()
{
	
}

void SceneManager::loadScene(Hash hash)
{
	engine.assetManager.loadSceneFile(hash);
}