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
public:
	
};