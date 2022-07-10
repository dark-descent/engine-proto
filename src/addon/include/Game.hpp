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
	std::filesystem::path gamePath_;
	std::unordered_map<Hash, Game::SceneInfo> scenes_;

public:
	Game(std::string& gameFile);

	const char* const getSceneFileName(Hash sceneName);
	const char* const getSceneFileName(const char* sceneName);

	const std::string getGamePath();
};