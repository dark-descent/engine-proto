#include "Arch.hpp"


size_t Arch::getOffsetIndex(size_t newArchBitMask)
{
	//find the index of the changed bit
	size_t testA = newArchBitMask;
	size_t testB = bitMask;
	uint16_t splitIndex = 0;
	for (splitIndex; splitIndex < 64 + 1; splitIndex++)
	{
		if ((testA & 1) != (testB & 1))
			break;
		testA >>= 1;
		testB >>= 1;
	}

	const size_t maxIndex = offsets.size() - 1;

	if (splitIndex > maxIndex)
		splitIndex = maxIndex;

	return splitIndex;
}

const size_t Arch::getComponentCount() const 
{
	return offsets.size() - 1;
}
