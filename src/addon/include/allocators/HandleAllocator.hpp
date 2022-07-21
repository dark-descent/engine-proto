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

	T* operator->()
	{ 
		return &data; 
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
		Handle<T>* ptr = static_cast<Handle<T>*>(aligned_malloc(0x10, bufferSize()));
		memset(ptr, 0, bufferSize());
		return ptr;
	}

public:
	size_t size()
	{
		return ((insertIndex_.buffer * Count) + insertIndex_.index) - freeIndices_.size();
	}


	HandleAllocator() : insertIndex_({ -1, 0 }), buffers_(), freeIndices_() {  }

	HandleAllocator(HandleAllocator&& other)
	{
		insertIndex_ = std::move(other.insertIndex_);
		buffers_ = std::move(other.buffers_);
		freeIndices_ = std::move(other.freeIndices_);
	}

	HandleAllocator(const HandleAllocator& other)
	{
		insertIndex_ = other.insertIndex_;
		buffers_ = other.buffers_;
		freeIndices_ = other.freeIndices_;
	}

	~HandleAllocator()
	{
		clear();
	}

	void clear()
	{
		for (auto& ptr : buffers_)
			aligned_free(ptr);
		insertIndex_= { -1, 0 };
	}
	
	Handle<T>& at(size_t bufferIndex, size_t index)
	{
		return buffers_[bufferIndex][index];
	}
	
	Handle<T>& at(HandleIndex index)
	{
		return at(index.buffer, index.index);
	}

	Handle<T>& at(size_t index)
	{
		const size_t bi = index / bufferSize();
		const size_t i = index % bufferSize();
		return at(bi, i);
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
		handle->index = { bi, i };
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

	template<typename Callback>
	void iterate(Callback callback)
	{
		const size_t count = size();
		size_t counter = 0;
		for(Handle<T>* buffer : buffers_)
		{
			for(size_t i = 0; i < Count; i++)
			{
				Handle<T>& handle = buffer[i];
				if(handle.index.buffer != -1)
				{
					callback(handle, counter++);
					if(counter >= count)
						goto exitLoop;
				}		
				
			}
		}

		exitLoop:
			return;
	}
};