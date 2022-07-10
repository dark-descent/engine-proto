#pragma once

class ArchType
{
	size_t bitMask_;
	size_t size_;

public:
	ArchType(size_t bitMask, size_t size);
};