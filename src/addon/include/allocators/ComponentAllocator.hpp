#pragma once

#include "framework.hpp"
#include "allocators/AllocIndex.hpp"

class ComponentAllocator
{
	AllocIndex insertIndex_;
	size_t size_;
	size_t capacity_;
	size_t bufferSize_;

	std::vector<char*> buffers_;

	inline char* allocBuffer()
	{
		puts("allocated component buffer\n");
		char* buffer = static_cast<char*>(aligned_malloc(0x10, bufferSize_));
		buffers_.emplace_back(buffer);
		return buffer;
	}

public:
	ComponentAllocator(const size_t size, const size_t count = 1024) : size_(size), capacity_(count), bufferSize_(size* count), insertIndex_({ -1, 0 }) { }
	ComponentAllocator() : ComponentAllocator(0, 1024) { }
	
	~ComponentAllocator()
	{
		clear();
	}

	void reset(const size_t size, const size_t count = 1024)
	{
		clear();
		size_ = size;
		capacity_ = count;
		bufferSize_ = size * count;
	}

	void clear()
	{
		for (auto& ptr : buffers_)
		{
			puts("freed component buffer\n");
			aligned_free(ptr);
		}
		insertIndex_= { -1, 0 };
	}
	

	size_t size()
	{
		int64_t count = ((insertIndex_.buffer * capacity_) + insertIndex_.index) - 1;
		return count < 0 ? 0 : count;
	}

	AllocIndex alloc()
	{
		if(size_ == 0)
			throw std::runtime_error("Cannot alloc component with size 0!");
			
		if (insertIndex_.buffer == -1)
		{
			allocBuffer();
			insertIndex_.buffer++;
		}

		int32_t bi = insertIndex_.buffer;
		int32_t i = insertIndex_.index++;

		if (insertIndex_.index >= capacity_)
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			allocBuffer();
		}

		return { bi, i };
	}

	char* at(AllocIndex& index)
	{
		return static_cast<char*>(buffers_[index.buffer] + (size_ * index.index));
	}

	size_t bufferSize()
	{
		return bufferSize_;
	}
};