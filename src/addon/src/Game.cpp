#include "framework.hpp"
#include "Game.hpp"

Game::Game()
{
#ifdef _DEBUG
	gameDir_ = std::filesystem::current_path() / "test-project";
#else
	gameDir_ = std::filesystem::current_path();
#endif

	std::filesystem::path gameFilePath = gameDir_ / "game.sb";

	
}

const char* const Game::getSceneFileName(const char* sceneName)
{
	return getSceneFileName(Hasher::hash(sceneName));
}

const char* const Game::getSceneFileName(Hash sceneName)
{
	// if (scenes_.contains(sceneName))
	// 	return scenes_.at(sceneName).fileName.c_str();
	return nullptr;
}

const std::string Game::getGameDir()
{
	return gameDir_.string<char>();
}