#pragma once

#include "framework.hpp"

using ComponentIndex = uint64_t;

struct ComponentLayout
{
	size_t index;
	size_t size;

	// ComponentLayout(size_t index, size_t size) : index(index), size(size) {}
};

class Component
{
	size_t bitMask_;
	size_t size_;

public:
	Component(size_t bitMask, size_t size);

	const size_t getSize() const;
	const size_t getBitMask() const;
};