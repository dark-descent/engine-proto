#pragma once

#include <array>
#include <vector>

template<typename T, size_t BufferCapacity = 1024>
class AlignedAllocator
{
private:
	struct InsertIndex
	{
		size_t buffer;
		size_t index;
	};

	std::vector<std::array<T, BufferCapacity>> buffers_;
	size_t insertIndex_ = 0;

public:
	AlignedAllocator() : buffers_()
	{
		std::array<T, BufferCapacity> buffer = { 0 };
		buffers_.push_back(buffer);
	}

	size_t alloc()
	{
		size_t index = insertIndex_++;
		if (insertIndex_ % BufferCapacity == 0)
		{
			std::array<T, BufferCapacity> buffer = { 0 };
			buffers_.push_back(buffer);
		}
		return index;
	}

	void free(size_t index)
	{

	}

	T* at(size_t index)
	{
		return &static_cast<T*>(buffers_[0].data())[index];
	}
};