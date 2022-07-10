#include "framework.hpp"
#include "Game.hpp"

Game::Game(std::string& fileName) : scenes_()
{
#ifdef _DEBUG
	gamePath_ = std::filesystem::current_path() / "test-project";
#else
	gamePath_ = std::filesystem::current_path();
#endif

	printf("%s\n", getGamePath().c_str());

	std::filesystem::path gameFilePath = gamePath_ / fileName;

	std::string gameFilePathString = gameFilePath.string<char>() + ".sb";
	std::transform(gameFilePathString.begin(), gameFilePathString.end(), gameFilePathString.begin(), [](char c) { return std::tolower(c); });

	std::ifstream input(gameFilePathString, std::ifstream::binary);

	if (!input)
		throw std::runtime_error("Game file does not exists!");

	input.seekg(0, input.end);
	const int maxSize = input.tellg();
	input.seekg(0);

	if (maxSize <= 0)
		throw std::runtime_error("Game file does not exists or is empty!");

	size_t rowStartOffset = 0;
	bool isSceneNameTarget = true;

	uint64_t sceneID = 0;
	std::string sceneName = std::string();
	std::string sceneFileName = std::string();

	size_t parsedSize = 0;

	std::vector<char> buffer(1024, 0);

	std::vector<std::pair<std::string, std::string>> scenes;

	while (parsedSize < maxSize)
	{
		input.read(buffer.data(), 1024);

		std::streamsize dataSize = input.gcount();

		if (dataSize <= 1024)
			buffer[dataSize] = '\0';

		for (int i = 0; i < dataSize; i++)
		{
			size_t off = (parsedSize + i) - rowStartOffset;

			if (off < 4)
			{
				sceneID <<= 8;
				sceneID |= buffer[i];
			}
			else if (isSceneNameTarget)
			{
				sceneName.push_back(buffer[i]);
				if (buffer[i] == '\0')
				{
					isSceneNameTarget = false;
				}
			}
			else
			{
				sceneFileName.push_back(buffer[i]);
				if (buffer[i] == '\0')
				{
					isSceneNameTarget = true;
					Game::SceneInfo si;
					si.fileName = std::string(sceneFileName.data());
					si.name = std::string(sceneName.data());
					scenes_.emplace(std::make_pair(Hasher::hash(sceneName.data()), si));
					sceneID = 0;
					sceneName.clear();
					sceneFileName.clear();
					rowStartOffset = i + 1;
				}
			}
		}

		parsedSize += 1024;
	}

	input.close();
}

const char* const Game::getSceneFileName(const char* sceneName)
{
	return getSceneFileName(Hasher::hash(sceneName));
}

const char* const Game::getSceneFileName(Hash sceneName)
{
	if (scenes_.contains(sceneName))
		return scenes_.at(sceneName).fileName.c_str();
	return nullptr;
}

const std::string Game::getGamePath()
{
	return gamePath_.string<char>();
}