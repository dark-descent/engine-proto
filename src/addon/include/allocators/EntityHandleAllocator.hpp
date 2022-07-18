#pragma once

#include <array>
#include <vector>
#include <stack>

#include "Entity.hpp"

template<size_t Count = 1024>
class EntityHandleAllocator
{
private:
	EntityHandleIndex insertIndex_;
	std::vector<EntityHandle*> buffers_;
	std::stack<EntityHandleIndex> freeIndices_;

	constexpr size_t bufferSize() { return Count * sizeof(EntityHandle); }

	inline EntityHandle* allocBuffer()
	{
		return static_cast<EntityHandle*>(malloc(bufferSize()));
	}

	size_t size()
	{
		return (insertIndex_.buffer * Count) + insertIndex_.index;
	}

	friend class Engine;

public:
	EntityHandleAllocator() : insertIndex_({ -1, 0 }), buffers_(), freeIndices_()
	{
		
	}

	EntityHandle* at(size_t index)
	{
		const size_t bi = index / bufferSize();
		const size_t i = index % bufferSize();
		return &buffers_[bi][i];
	}

	EntityHandle& alloc()
	{
		if (insertIndex_.buffer == -1)
		{
			buffers_.emplace_back(allocBuffer());
			insertIndex_.buffer++;
		}

		int32_t bi = insertIndex_.buffer;
		int32_t i = insertIndex_.index++;
		EntityHandle* handle = &buffers_[bi][i];
		handle->index = { bi, i };
		if (insertIndex_.index >= bufferSize())
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.emplace_back(allocBuffer());
		}
		return *handle;
	}

	void free(EntityHandle& handle)
	{
		int32_t bi, i;

		if(insertIndex_.index == 0)
		{
			bi = insertIndex_--;
			i = insertIndex_.index;
		}
		else
		{
			bi = insertIndex_.buffer;
			i = insertIndex_.index - 1;
		}

		if(bi < 0)
			throw std::runtime_error("Tried to free already freed handle!");

		if(handle.index.buffer == bi && handle.index.index == i)
			insertIndex_ = { bi, i };
		else
			freeIndices_.emplace(handle.index);

		handle.index = { -1, 0 };
	}
};