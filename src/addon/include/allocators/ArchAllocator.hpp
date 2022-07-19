#pragma once

#include "framework.hpp"

class ArchAllocator
{
	struct Index
	{
		int32_t buffer;
		int32_t index;
	};

	Index insertIndex_;
	const size_t size_;
	const size_t capacity_;
	const size_t bufferSize_;

	std::vector<char*> buffers_;

	inline char* allocBuffer()
	{
		return static_cast<char*>(aligned_malloc(0x10, bufferSize_));
	}

public:
	ArchAllocator(const size_t size, const size_t count) : size_(size), capacity_(count), bufferSize_(size * count), insertIndex_({ -1, 0 })
	{

	}

	size_t size()
	{
		int64_t count = ((insertIndex_.buffer * capacity_) + insertIndex_.index) - 1;
		return count < 0 ? 0 : count;
	}

	void* alloc()
	{
		if (insertIndex_.buffer == -1)
		{
			buffers_.emplace_back(allocBuffer());
			insertIndex_.buffer++;
		}

		int32_t bi = insertIndex_.buffer;
		int32_t i = insertIndex_.index++;
		
		if (insertIndex_.index >= capacity_)
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.emplace_back(allocBuffer());
		}

		return static_cast<void*>(buffers_[bi] + size_);
	}
};