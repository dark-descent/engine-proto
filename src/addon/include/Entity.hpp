#pragma once


#include "ArchType.hpp"

PACK(struct EntityHandleIndex
{
	int32_t buffer;
	int32_t index;
});

struct EntityHandle
{
	EntityHandleIndex index;
	void* entity;
	ArchTypeIndex archType;

	EntityHandle(EntityHandleIndex index, void* entity, ArchTypeIndex archType) : index(index), entity(entity), archType(archType) { }

	EntityHandle& operator=(const EntityHandle& other)
	{
		entity = other.entity;
		archType = other.archType;
		return *this;
	}
};