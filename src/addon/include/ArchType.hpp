#pragma once

#include "framework.hpp"

#include "Component.hpp"
#include "Logger.hpp"
#include "allocators/ArchAllocator.hpp"

using ArchTypeIndex = size_t;

struct ArchTypeArm
{
	ArchTypeIndex index;
	uint16_t offset; // the offset "index" where the component data needs to be split

	ArchTypeArm() : index(0), offset(0) { }
	ArchTypeArm(ArchTypeIndex index, uint16_t offset) : index(index), offset(offset) { }
};

class ArchType
{
public:
	ArchTypeIndex index;
	size_t bitMask;
	size_t size;
	std::vector<ArchTypeArm> remove;
	std::vector<ArchTypeArm> add;
	std::vector<uint16_t> componentOffsets;
	ArchAllocator allocator_;

	ArchType(ArchTypeIndex index, size_t bitMask, size_t size, std::vector<uint16_t>& offsets);

	size_t getOffsetOf(size_t component);

	size_t getSplitIndex(size_t newArchBitMask, size_t componentIndex);

	void* alloc();
	size_t getEntityCount();

	void log();
};
