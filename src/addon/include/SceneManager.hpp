#pragma once

#include "SubSystem.hpp"
#include "Hasher.hpp"
#include "Scene.hpp"

SUB_SYSTEM_CLASS(SceneManager)
{
private:
	static inline std::string createScenePath(size_t index)
	{
		char buf[256] = { 0 };
		snprintf(buf, sizeof(buf), "%zu", index);
		std::string scenePath = std::string(buf) + std::string(".bin");
		return scenePath;
	}

	Scene* activeScene_;

	std::unordered_map<Hash, Scene> scenes_;

	SUB_SYSTEM_OVERRIDES(SceneManager, activeScene_(nullptr), scenes_());

public:
	Scene& loadScene(const Hash name);

	Scene& addScene(std::string name, bool load = false);

	Scene& getActiveScene();
};