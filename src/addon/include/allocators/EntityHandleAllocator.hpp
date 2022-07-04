#pragma once

#include <array>
#include <vector>
#include <stack>

#include "Entity.hpp"
#include "allocators/EntityHandleAllocatorIndex.hpp"

class EntityHandleAllocator
{
private:
	EntityHandleAllocatorIndex insertIndex_ = { 0, 0 };
	std::vector<EntityHandle*> buffers_;
	std::stack<EntityHandleAllocatorIndex> freeIndices_;
	size_t bufferSize_ = 1024;

	inline EntityHandle* allocBuffer()
	{
		return static_cast<EntityHandle*>(malloc(sizeof(EntityHandle) * bufferSize_));
	}

	size_t size()
	{
		return (insertIndex_.buffer * bufferSize_) + insertIndex_.index;
	}

	friend class Engine;

public:
	EntityHandleAllocator(size_t bufferSize) : insertIndex_({ 0, 0 }), buffers_(), freeIndices_()
	{
		bufferSize_ = bufferSize;
		printf("EntityHandleAllocater initialized with buffersize: %zu\r\n", bufferSize);
		buffers_.push_back(allocBuffer());
	}

	EntityHandleAllocator(const EntityHandleAllocator&) = delete;
	EntityHandleAllocator(EntityHandleAllocator&&) = delete;

	EntityHandle* at(size_t index)
	{
		const size_t bi = index / bufferSize_;
		const size_t i = index % bufferSize_;
		return &buffers_[bi][i];
	}

	EntityHandle* alloc(size_t entityIndex)
	{
		int32_t bi = insertIndex_.buffer;
		int32_t i = insertIndex_.index++;
		EntityHandle* handle = &buffers_[bi][i];
		if (insertIndex_.index >= bufferSize_)
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.push_back(allocBuffer());
		}
		handle->entityIndex = entityIndex;
		handle->handleIndex = { bi, i };
		return handle;
	}

	void free(size_t index)
	{

	}

	void free(EntityHandle* handle)
	{
		const int32_t bi = handle->handleIndex.buffer;
		const int32_t i = handle->handleIndex.index;
		int32_t lbi = insertIndex_.buffer;
		int32_t li = insertIndex_.index - 1;

		if (li < 0)
		{
			lbi--;
			li = bufferSize_ - 1;
		}

		if (bi != lbi || li != i)
		{
			EntityHandleAllocatorIndex index = { bi, i };
			freeIndices_.push(index);
		}
		else
		{
			insertIndex_.buffer = lbi;
			insertIndex_.index = li;
		}

		handle->handleIndex = { -1, -1 };
	}

	template<typename Callback>
	void iterate(Callback callback)
	{
		const size_t maxI = size();
		size_t l = buffers_.size();
		size_t indexCounter = 0;
		for (size_t i = 0; i < l; i++)
		{
			EntityHandle* buffer = buffers_.data()[i];
			for (size_t j = 0; j < bufferSize_; j++)
			{
				indexCounter++;
				if (indexCounter >= maxI)
					goto exitLoop;
				EntityHandle* entity = &buffer[j];
				if (entity->handleIndex.index > -1 && entity->handleIndex.buffer > -1)
				{
					callback(entity, indexCounter);
				}
			}
		}

	exitLoop:
		return;
	}
};