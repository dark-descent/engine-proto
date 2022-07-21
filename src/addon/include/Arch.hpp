#pragma once

#include "framework.hpp"
#include "allocators/HandleAllocator.hpp"
#include "allocators/ComponentAllocator.hpp"

struct Arch;

struct ArchArm
{
	Handle<Arch>* arch;
	size_t offsetIndex;

	ArchArm() : arch(nullptr), offsetIndex(0) { }
	ArchArm(Handle<Arch>* arch, size_t offsetIndex) : arch(arch), offsetIndex(offsetIndex) { }
};

using ComponentOffsets = std::vector<uint16_t>;

struct Arch
{
	size_t bitMask;
	size_t size;

	std::vector<ArchArm> add;
	std::vector<ArchArm> remove;
	ComponentOffsets offsets;

	ComponentAllocator allocator;

	const size_t getComponentCount() const;

	Arch(size_t bitMask, size_t size, ComponentOffsets&& offsets) : bitMask(bitMask), size(size), offsets(offsets), allocator(size) { }
	Arch() : Arch(0, 0, { 0 }) { }

	size_t getOffsetIndex(size_t newArchBitMask);
};