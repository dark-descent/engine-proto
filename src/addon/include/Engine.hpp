#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Component.hpp"
#include "AssetManager.hpp"
#include "SceneManager.hpp"
#include "Game.hpp"
#include "Logger.hpp"

class Engine
{
private:
	static Engine* engine_;

public:
	static void initialize(V8CallbackArgs args);

private:
	static void destroy(void* data);

	bool isInitialized_ = false;

	std::vector<ComponentInfo> components_;
	std::vector<SubSystem*> subSystems_;

public:
	AssetManager assetManager;
	SceneManager sceneManager;

	Logger& logger;

	Game game;

private:
#ifdef _DEBUG
	std::vector<std::pair<Hash, std::string>> componentNames_;
#endif

	Engine(Config& config, ObjectBuilder& exports);
	~Engine();
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;

	template<typename T>
	size_t registerAndExposeComponent(ObjectBuilder& exports, const char* name)
	{
		const size_t index = components_.size();
		const uint64_t bitMask = 1ULL << index;
		components_.emplace_back(index, bitMask, sizeof(T));

#ifdef _DEBUG
		const Hash hash = Hasher::hash<T>();
		const char* componentName = typeid(T).name();

		bool found = false;
		for (const auto& item : componentNames_)
		{
			if (item.first == hash)
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			logger.warn("Component ", name, " is already registered!");
		}
		else
		{
			logger.info("Registering component ", name, " at [ index: ", index, ", bitMask: ", bitMask, " ]");
			componentNames_.push_back(std::make_pair(hash, componentName));
		}
#endif

		return index;
	}

public:
	const size_t getComponentSize(size_t index);
	const size_t getComponentBitMask(size_t index);
	const ComponentInfo& getComponent(size_t index);
};