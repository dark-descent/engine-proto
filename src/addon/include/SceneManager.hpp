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
	Scene& loadScene(Hash name);

	template <bool load = false>
	auto addScene(std::string name) -> typename std::conditional<load, Scene&, void>::type
	{
		Hash h = Hasher::hash(name.c_str());
		if (scenes_.contains(h))
			throw std::runtime_error("Scene already exists!");
		scenes_.emplace(std::make_pair(h, Scene(name, createScenePath(scenes_.size() + 1))));

		if constexpr (load)
		{
			activeScene_ = std::addressof(scenes_.at(h));
			return *activeScene_;
		}
	};

	Scene& getActiveScene();
};