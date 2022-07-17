#pragma once

class Component
{
	size_t bitMask_;
	size_t size_;

public:
	Component(size_t bitMask, size_t size);

	const size_t getSize();
	const size_t getBitMask();
};