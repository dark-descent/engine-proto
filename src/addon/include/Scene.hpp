#pragma once

#include "framework.hpp"
#include "ArchType.hpp"
#include "Entity.hpp"
#include "allocators/EntityHandleAllocator.hpp"

class Engine;

class Scene
{
private:
	Engine& engine_;
	std::string name_;
	std::string path_;

	std::vector<ArchType> archTypes_;
	EntityHandleAllocator<1024> entityHandles_;

	inline ArchTypeIndex addArchType(size_t bitMask);

public:
	Scene(Engine& engine, std::string name, std::string path);
	Scene(Scene&& other);
	Scene(const Scene& other);

	const std::string& name();
	const std::string& path();

	EntityHandle& addEntity(std::string name = "GameObject");

	void addComponentToEntity(EntityHandle& entity, size_t component);

	ArchType& getArchType(size_t index);
	ArchType& getRootArchType();
};