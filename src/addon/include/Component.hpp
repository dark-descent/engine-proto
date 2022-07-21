#pragma once

#include "framework.hpp"

using ComponentIndex = uint64_t;

class ComponentInfo
{
	ComponentIndex index_;
	size_t bitMask_;
	size_t size_;

public:
	ComponentInfo(ComponentIndex index, size_t bitMask, size_t size);

	const ComponentIndex getIndex() const;
	const size_t getSize() const;
	const size_t getBitMask() const;
};