#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Component.hpp"
#include "ArchType.hpp"
#include "AssetManager.hpp"
#include "SceneManager.hpp"
#include "Game.hpp"


class Engine
{
private:
	static Engine* engine_;
	static uint8_t componentBitMaskCounter_;

public:
	static void initialize(V8CallbackArgs args);

private:
	static void destroy(void* data);

	bool isInitialized_ = false;
	std::unordered_map<uint64_t, ArchType> archTypes_;
	std::vector<Component> components_;
	std::vector<SubSystem*> subSystems_;

public:
	AssetManager assetManager;
	SceneManager sceneManager;
	
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
	uint64_t registerAndExposeComponent(ObjectBuilder& exports, const char* name)
	{
		const uint64_t bitMask = componentBitMaskCounter_;
		const size_t index = components_.size();
		components_.push_back(Component(bitMask, sizeof(T)));
		componentBitMaskCounter_ <<= 1;

#ifdef _DEBUG
		const Hash hash = Hasher::hash<T>();
		const char* componentName = typeid(T).name();
		
		bool found = false;
		for(const auto& item : componentNames_)
		{
			if(item.first == hash)
			{
				found = true;
				break;
			}
		}
		
		if(found)
		{
			printf("Component %s is already registered!\n", name);
		}
		else
		{
			printf("Registering component %s at [ index: %zu, bitMask: %zu ]\n", name, index, bitMask);
			componentNames_.push_back(std::make_pair(hash, componentName));
		}
#endif
		return bitMask;
	}
};