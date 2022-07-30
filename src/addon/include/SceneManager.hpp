#pragma once

#include "SubSystem.hpp"
#include "Hasher.hpp"
#include "Scene.hpp"

SUB_SYSTEM_CLASS(SceneManager)
{
private:
	static void addSceneCallback(V8CallbackArgs args);
	static void addEntityCallback(V8CallbackArgs args);
	static void removeSceneCallback(V8CallbackArgs args);
	static void loadEditorSceneCallback(V8CallbackArgs args);

	constexpr static size_t sceneBufferSize = 64;

	static inline std::string createScenePath(size_t index)
	{
		char buf[256] = { 0 };
		snprintf(buf, sizeof(buf), "%zu", index);
		std::string scenePath = std::string(buf) + std::string(".bin");
		return scenePath;
	}

	bool isLoaded_;

	size_t activeScene_;

	std::unordered_map<Hash, size_t> scenesIndices_;
	std::vector<Scene> scenes_;
	std::stack<size_t> freeSceneIndices_;

	SUB_SYSTEM_OVERRIDES(SceneManager, isLoaded_(false), activeScene_(0), scenesIndices_(), scenes_(), freeSceneIndices_());

private:
	size_t loadEditorScene(const size_t index);

public:
	size_t loadScene(const Hash name);

	size_t addScene(std::string name, bool load = false);
	size_t addScene(std::string name, std::string path, bool load = false);

	void removeScene(const size_t index);

	Scene& getActiveScene();

	size_t getSceneIndex(const Hash hash);
	Scene& getSceneFromHash(const Hash hash);
	Scene& getScene(const size_t index);

};