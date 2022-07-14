#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Bin.hpp"

class Game
{
private:
	STATIC_BIN_TEMPLATE(GameData, gameDataParser, {
		Bin::vector<Bin::string> scenes;
	});

	std::filesystem::path gameDir_;
	
	GameData data_;
	
public:
	Game();

	const char* const getSceneFileName(Hash sceneName);
	const char* const getSceneFileName(const char* sceneName);

	const std::string getGameDir();
};