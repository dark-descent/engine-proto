#pragma once

#include "framework.hpp"
#include "ArchType.hpp"
#include "Entity.hpp"
#include "allocators/HandleAllocator.hpp"

class Engine;

class Scene
{
private:
	Engine& engine_;
	std::string name_;
	std::string path_;

	std::vector<ArchType> archTypes_;
	HandleAllocator<Entity, 1024> entityHandles_;

	inline ArchTypeIndex addArchType(size_t bitMask);

public:
	Scene(Engine& engine, std::string name, std::string path);
	Scene(Scene&& other);
	Scene(const Scene& other);

	const std::string& name();
	const std::string& path();

	Handle<Entity>& addEntity(std::string name = "GameObject");

	void addComponentToEntity(Handle<Entity>& entity, size_t component);

	ArchType& getArchType(size_t index);
	ArchType& getRootArchType();
};