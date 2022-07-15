#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Bin.hpp"

class Game
{
private:
	std::filesystem::path gameDir_;

	std::unordered_map<Hash, std::string> scenePaths_;
	
public:
	Game();

	const char* const getSceneFileName(Hash sceneName);
	const char* const getSceneFileName(const char* sceneName);

	const std::string getGameDir();
};