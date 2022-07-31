#pragma once

#include "framework.hpp"
#include "Hasher.hpp"
#include "Component.hpp"
#include "AssetManager.hpp"
#include "SceneManager.hpp"
#include "Renderer.hpp"
#include "Game.hpp"
#include "Logger.hpp"

class System;

class Engine
{
private:
	static Engine* engine_;

public:
	static void initialize(V8CallbackArgs args);
	static void initializeWorker(V8CallbackArgs args);
	static Engine* get();

private:
	static void destroy(void* data);

	bool isInitialized_ = false;

	std::vector<ComponentInfo> components_;
	std::vector<JsClass*> jsComponents_;
	std::vector<SubSystem*> subSystems_;

	std::unordered_map<Hash, size_t> componentTypeMap_;

	v8::Isolate* isolate_;

public:
	JsEntity jsEntity;
	AssetManager assetManager;
	SceneManager sceneManager;
	Renderer renderer;

	Logger& logger;

	Game game;

	v8::Isolate* getIsolate();
	v8::Local<v8::Context> getContext();

private:
#ifdef _DEBUG
	std::vector<std::pair<Hash, std::string>> componentNames_;
#endif

	Engine(v8::Isolate* isolate, Config& config, ObjectBuilder& exports);
	~Engine();
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;

	template<typename Component, class JsComponentType>
	size_t registerAndExposeComponent(ArrayBuilder& exports, const char* name)
	{
		const size_t index = components_.size();
		const uint64_t bitMask = 1ULL << index;

		components_.emplace_back(index, bitMask, sizeof(Component));
		componentTypeMap_.emplace(std::make_pair(Hasher::hash(typeid(Component).name()), index));

		auto jsType = new JsComponentType(*this, name);
		jsType->init(isolate_);
		jsComponents_.emplace_back(jsType);

#ifdef _DEBUG
		const Hash hash = Hasher::hash<Component>();
		const char* componentName = typeid(Component).name();

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

		exports.pushObject([&](ObjectBuilder& obj)
		{
			obj.set("name", name);
			obj.set("index", index);
			obj.setVal("type", jsType->getClass(isolate_));
		});

		return index;
	}

public:
	const size_t getComponentSize(size_t index);
	const size_t getComponentBitMask(size_t index);
	const ComponentInfo& getComponent(size_t index);

	template<typename T>
	const ComponentInfo& getComponent()
	{
		Hash h = Hasher::hash(typeid(T).name());
		if (!componentTypeMap_.contains(h))
			throw std::runtime_error("Could not get Component!");
		return getComponent(componentTypeMap_.at(h));
	}

	void runSystem(System& system);
};