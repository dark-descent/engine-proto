#pragma once

#include "allocators/EntityHandleAllocatorIndex.hpp"
#include "ComponentFamily.hpp"

struct Component;

struct EntityHandle
{
	size_t entityIndex;
	EntityHandleAllocatorIndex handleIndex;
};

struct Entity
{
	Component* firstComponent = nullptr;
	Component* lastComponent = nullptr;
	ComponentFamily family = 0;
	EntityHandle* handle = nullptr;
};