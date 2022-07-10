#pragma once

#include "SubSystem.hpp"
#include "Hasher.hpp"

class AssetManager : public SubSystem
{
private:
	std::filesystem::path scenesPath_;

	SUB_SYSTEM_OVERRIDES(AssetManager);

public:
	template<typename T>
	bool loadFile(const char* path, T callback)
	{
		return loadFile(path, 1024, callback);
	}

	template<typename T>
	bool loadFile(const char* path, const size_t chunkSize, T callback)
	{
		std::ifstream input(path, std::ifstream::binary);

		if (!input)
			return false;

		input.seekg(0, input.end);
		const int maxSize = input.tellg();
		input.seekg(0);

		if (maxSize <= 0)
			return false;

		size_t parsedSize = 0;

		std::vector<char> buffer(chunkSize, 0);

		while (parsedSize < maxSize)
		{
			input.read(buffer.data(), chunkSize);

			std::streamsize dataSize = input.gcount();

			callback(buffer.data(), dataSize, parsedSize);

			parsedSize += chunkSize;
		}

		return true;
	}

public:
	void loadSceneFile(Hash sceneName);
};