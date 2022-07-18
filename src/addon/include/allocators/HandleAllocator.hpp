#pragma once

#include "framework.hpp"


struct HandleIndex
{
	int32_t buffer;
	int32_t index;
};

template<typename T>
struct Handle
{
	HandleIndex index;
	T data;

	T& operator*()
	{
		return data;
	}
};

template<typename T, size_t Count = 1024>
class HandleAllocator
{
	HandleIndex insertIndex_;
	std::vector<Handle<T>*> buffers_;
	std::stack<HandleIndex> freeIndices_;

	constexpr size_t bufferSize() { return Count * sizeof(Handle<T>); }

	inline Handle<T>* allocBuffer()
	{
		return static_cast<Handle<T>*>(malloc(bufferSize()));
	}

public:
	size_t size()
	{
		return (insertIndex_.buffer * Count) + insertIndex_.index;
	}


	HandleAllocator() : insertIndex_({ -1, 0 }), buffers_(), freeIndices_()
	{

	}

	Handle<T>& at(size_t index)
	{
		const size_t bi = index / bufferSize();
		const size_t i = index % bufferSize();
		return buffers_[bi][i];
	}

	Handle<T>& alloc()
	{
		if (insertIndex_.buffer == -1)
		{
			buffers_.emplace_back(allocBuffer());
			insertIndex_.buffer++;
		}

		int32_t bi = insertIndex_.buffer;
		int32_t i = insertIndex_.index++;

		Handle<T>* handle = &buffers_[bi][i];
		printf("Set index :D\n");
		handle->index = { bi, i };
		printf("Index set ! :D\n");
		if (insertIndex_.index >= bufferSize())
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.emplace_back(allocBuffer());
		}

		return *handle;
	}

	void free(Handle<T>& handle)
	{
		int32_t bi, i;

		if (insertIndex_.index == 0)
		{
			bi = insertIndex_--;
			i = insertIndex_.index;
		}
		else
		{
			bi = insertIndex_.buffer;
			i = insertIndex_.index - 1;
		}

		if (bi < 0)
			throw std::runtime_error("Tried to free already freed handle!");

		if (handle.index.buffer == bi && handle.index.index == i)
			insertIndex_ = { bi, i };
		else
			freeIndices_.push(handle.index);

		handle.index = { -1, 0 };
	}
};