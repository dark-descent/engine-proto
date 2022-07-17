#pragma once

struct ArchType;

struct EntityHandle
{
	void* entity;
	ArchType* archType;

	EntityHandle(void* entity, ArchType* archType) : entity(entity), archType(archType) {}

	EntityHandle& operator=(const EntityHandle & other)
	{
		entity = other.entity;
		archType = other.archType;
		return *this;
	}
};