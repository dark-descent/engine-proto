#pragma once

#include "framework.hpp"
#include "Hasher.hpp"

class Game
{
public:
	struct SceneInfo
	{
		std::string name;
		std::string fileName;
	};

private:
	std::filesystem::path gameDir_;
	std::unordered_map<Hash, Game::SceneInfo> scenes_;

public:
	Game();

	const char* const getSceneFileName(Hash sceneName);
	const char* const getSceneFileName(const char* sceneName);

	const std::string getGameDir();
};