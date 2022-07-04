#pragma once

#include <array>
#include <vector>
#include <stack>

#include "Entity.hpp"

class EntityHandleAllocator
{
private:
	struct Index
	{
		uint32_t buffer;
		uint32_t index;
	};

	Index insertIndex_ = { 0, 0 };
	std::vector<EntityHandle*> buffers_;
	std::stack<Index> freeIndices_;
	size_t bufferSize_ = 1024;

	inline EntityHandle* allocBuffer()
	{
		return static_cast<EntityHandle*>(malloc(sizeof(EntityHandle) * bufferSize_));
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

	EntityHandle* alloc(EntityHandle handleValue)
	{
		EntityHandle* handle = &buffers_[insertIndex_.buffer][insertIndex_.index++];
		if(insertIndex_.index >= bufferSize_)
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.push_back(allocBuffer());
		}
		*handle = handleValue;
		return handle;
	}

	void free(size_t index)
	{
		
	}
};