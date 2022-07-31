#pragma once

#include "framework.hpp"


struct HandleIndex
{
	constexpr static uint64_t serialize(HandleIndex& i) 
	{
		uint64_t serializedIndex = i.buffer << sizeof(uint32_t);
		return serializedIndex | i.index;
	}

	constexpr static HandleIndex parse(uint64_t i) 
	{
		HandleIndex index;
		index.index = i;
		index.buffer = i >> 32;
		return index;
	}

	constexpr static HandleIndex& parse(HandleIndex& index, uint64_t i) 
	{
		index.index = i;
		index.buffer = i >> 32;
		return index;
	}

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
		printf("Alloc buffer [HandleAllocator] %s\n", typeid(T).name());
		Handle<T>* ptr = static_cast<Handle<T>*>(aligned_malloc(0x10, bufferSize()));
		// memset(ptr, 0, bufferSize());
		buffers_.emplace_back(ptr);
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
		printf("Clear buffers [HandleAllocator] %s\n", typeid(T).name());
		for (auto& ptr : buffers_)
		{
			printf("Free buffer [HandleAllocator] %s\n", typeid(T).name());
			aligned_free(ptr);
		}
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
			allocBuffer();
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
			allocBuffer();
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