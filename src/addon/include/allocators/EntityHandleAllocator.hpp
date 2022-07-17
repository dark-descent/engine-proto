#pragma once

#include <array>
#include <vector>
#include <stack>

#include "Entity.hpp"

template<size_t Count = 1024>
class EntityHandleAllocator
{
public:
	PACK(struct Index {
		int32_t buffer;
		int32_t index;
	});

private:
	Index insertIndex_;
	std::vector<EntityHandle*> buffers_;
	std::stack<Index> freeIndices_;

	constexpr size_t bufferSize() { return Count * sizeof(EntityHandle); }

	inline EntityHandle* allocBuffer()
	{
		return static_cast<EntityHandle*>(malloc(bufferSize()));
	}

	size_t size()
	{
		return (insertIndex_.buffer * bufferSize()) + insertIndex_.index;
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
		printf("alloc at %i, %i\n", bi, i);
		if (insertIndex_.index >= bufferSize())
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.emplace_back(allocBuffer());
			printf("new buffer at %i, %i\n", bi, i);
		}
		return *handle;
	}

	void free(size_t index)
	{

	}

	void free(EntityHandle* handle)
	{
		// const int32_t bi = handle->handleIndex.buffer;
		// const int32_t i = handle->handleIndex.index;
		// int32_t lbi = insertIndex_.buffer;
		// int32_t li = insertIndex_.index - 1;

		// if (li < 0)
		// {
		// 	lbi--;
		// 	li = bufferSize_ - 1;
		// }

		// if (bi != lbi || li != i)
		// {
		// 	EntityHandleAllocatorIndex index = { bi, i };
		// 	freeIndices_.push(index);
		// }
		// else
		// {
		// 	insertIndex_.buffer = lbi;
		// 	insertIndex_.index = li;
		// }

		// handle->handleIndex = { -1, -1 };
	}
};