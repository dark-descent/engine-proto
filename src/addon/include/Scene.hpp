#pragma once

#include "framework.hpp"
#include "Arch.hpp"
#include "Entity.hpp"
#include "allocators/HandleAllocator.hpp"
#include "Component.hpp"
#include "System.hpp"

class Engine;

class Scene
{
private:
	Engine& engine_;
	std::string name_;
	std::string path_;

	HandleAllocator<Entity, 1024> entityHandles_;
	HandleAllocator<Arch, 128> archHandles_;
	std::vector<std::vector<Handle<Arch>*>> archLevels_;
	Handle<Arch>* rootArch_;

	inline Handle<Arch>& addArch(const size_t componentIndex, const size_t bitMask, const size_t size, const size_t level);

public:
	Scene(Engine& engine, std::string name, std::string path);

	void load(bool parse = true);
	void unload();

	bool isLoaded();

	const char* name();
	const char* path();

	Handle<Entity>& addEntity(std::string& name);
	Handle<Entity>& addEntity(const char* name = "Entity");

	void* addComponentToEntity(Handle<Entity>& handle, const ComponentIndex& componentIndex, void* data);
	void* addComponentToEntity(Handle<Entity>& handle, const ComponentInfo& component, void* data);

	Handle<Arch>& getArch(HandleIndex index);
	Handle<Arch>* getArchPtr(HandleIndex index);

	size_t filterArchTypes(size_t bitMask, ArchGroup& archTypes);
};