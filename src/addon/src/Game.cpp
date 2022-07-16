#include "framework.hpp"
#include "Game.hpp"

Game::Game() : sceneNames_()
{
#ifdef _DEBUG
	gameDir_ = std::filesystem::current_path() / "test-project";
#else
	gameDir_ = std::filesystem::current_path();
#endif

	std::filesystem::path gameFilePath = gameDir_ / "game.bin";

	Bin::Reader reader(gameFilePath.string());

	reader.read([&](Bin::Parser& parser)
	{
		parser.read(sceneNames_);
	});
}

const std::vector<std::string>& Game::getSceneNames()
{
	return sceneNames_;
}

const std::string Game::getGameDir()
{
	return gameDir_.string<char>();
}