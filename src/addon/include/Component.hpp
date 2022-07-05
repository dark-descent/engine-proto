#pragma once

#include "framework.hpp"
#include "allocators/AlignedAllocator.hpp"

struct Entity;

PACK(struct Component
{
	Entity* entity = nullptr;
	Component* nextComponent = nullptr;
	Component* prevComponent = nullptr;
	size_t componentIndex = 0;
	void* componentList = nullptr;
});