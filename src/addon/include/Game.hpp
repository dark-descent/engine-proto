#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Bin.hpp"

class Game
{
private:
	std::filesystem::path gameDir_;

	std::vector<std::string> sceneNames_;
	
public:
	Game();

	const std::vector<std::string>& getSceneNames();

	const std::string getGameDir();
};