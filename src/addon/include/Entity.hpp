#pragma once


#include "ArchType.hpp"


struct Entity
{
	void* entity;
	ArchTypeIndex archType;

	Entity(void* entity, ArchTypeIndex archType) : entity(entity), archType(archType) { }

	Entity& operator=(const Entity& other)
	{
		entity = other.entity;
		archType = other.archType;
		return *this;
	}
};