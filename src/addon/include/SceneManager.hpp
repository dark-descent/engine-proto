#pragma once

#include "SubSystem.hpp"
#include "Hasher.hpp"
#include "Scene.hpp"

SUB_SYSTEM_CLASS(SceneManager)
{
private:
	constexpr static size_t sceneBufferSize = 64;

	static inline std::string createScenePath(size_t index)
	{
		char buf[256] = { 0 };
		snprintf(buf, sizeof(buf), "%zu", index);
		std::string scenePath = std::string(buf) + std::string(".bin");
		return scenePath;
	}

	Scene* activeScene_;

	std::unordered_map<Hash, size_t> scenesIndices_;
	std::vector<Scene> scenes_;

	SUB_SYSTEM_OVERRIDES(SceneManager, activeScene_(nullptr), scenesIndices_(), scenes_());

public:
	Scene& loadScene(const Hash name);

	Scene& addScene(std::string name, bool load = false);
	Scene& addScene(std::string name, std::string path, bool load = false);

	Scene& getActiveScene();

	Scene& getScene(const Hash hash);
};