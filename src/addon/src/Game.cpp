#include "framework.hpp"
#include "Game.hpp"

Game::Game() : scenePaths_()
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
		std::vector<std::string> sceneNames;
		parser.read(sceneNames);

		size_t i = 1;
		char buf[256] = { 0 };
		for (const auto& str : sceneNames)
		{
			snprintf(buf, sizeof(buf), "%zu", i++);
			std::string scenePath = std::string(buf) + std::string(".bin");
			std::cout << "got scene " << str.c_str() << " with path scenes/" << scenePath.c_str() << std::endl;
			Hash h = Hasher::hash(str.c_str());
			scenePaths_.emplace(std::make_pair(h, scenePath));
		}
	});
}

const char* const Game::getSceneFileName(Hash sceneName)
{
	if (scenePaths_.contains(sceneName))
		return scenePaths_.at(sceneName).c_str();
	return nullptr;
}

const std::string Game::getGameDir()
{
	return gameDir_.string<char>();
}