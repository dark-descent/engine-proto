#include "ArchType.hpp"
#include <format>

ArchType::ArchType(ArchTypeIndex index, size_t bitMask, size_t size, std::vector<uint16_t>& componentOffsets) :
	index(index),
	bitMask(bitMask),
	size(size),
	remove(),
	add(),
	componentOffsets(std::move(componentOffsets)),
	allocator_(size, 256)
{

}

void* ArchType::alloc()
{
	return allocator_.alloc();
}

void ArchType::log()
{
	printf(
		"{\n"
		"    index: %zu,\n"
		"    bitMask: %zu,\n"
		"    size: %zu\n"
		"}",
		index, bitMask, size);
}

size_t ArchType::getOffsetOf(size_t component)
{
	size_t test = bitMask;
	size_t offsetCount = 0;

	for (size_t i = 0; i < component; i++)
	{
		if (test & 1)
			offsetCount++;
		test >>= 1;
	}

	return componentOffsets[offsetCount];
}

size_t ArchType::getSplitIndex(size_t newArchBitMask, size_t componentIndex)
{
	//find the index of the changed bit
	size_t testA = newArchBitMask;
	size_t testB = bitMask;
	uint16_t splitIndex = 0;
	for (splitIndex; splitIndex < componentIndex + 1; splitIndex++)
	{
		if ((testA & 1) != (testB & 1))
			break;
		testA >>= 1;
		testB >>= 1;
	}

	const size_t maxIndex = componentOffsets.size() - 1;

	if (splitIndex > maxIndex)
		splitIndex = maxIndex;

	return splitIndex;
}

size_t ArchType::getEntityCount()
{
	return allocator_.size();
}