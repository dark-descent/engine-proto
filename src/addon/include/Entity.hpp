#pragma once

#include "allocators/EntityHandleAllocatorIndex.hpp"

struct Component;

struct EntityHandle
{
	size_t entityIndex;
	EntityHandleAllocatorIndex handleIndex;
};

struct Entity
{
	Component* firstComponent = nullptr;
	EntityHandle* handle;
};