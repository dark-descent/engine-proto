#pragma once

#include "framework.hpp"

struct ArchType
{
	size_t bitMask;
	size_t size;

	std::vector<ArchType*> remove;
	std::vector<ArchType*> add;

	ArchType(size_t bitMask, size_t size);
};