#pragma once

#include <array>
#include <vector>

template<typename T>
class AlignedAllocator
{
private:
	struct Index
	{
		size_t buffer;
		size_t index;
	};

	std::vector<T*> buffers_;
	Index insertIndex_ = { 0, 0 };
	size_t bufferSize_;

	inline T* allocBuffer()
	{
		return static_cast<T*>(malloc(sizeof(T) * bufferSize_));
	}

	inline Index calcIndexStruct(size_t index)
	{
		return { index / bufferSize_, index % bufferSize_ };
	}

	inline size_t calcIndex(Index index)
	{
		return calcIndex(index.buffer, index.index);
	}

	inline size_t calcIndex(size_t bufferIndex, size_t index)
	{
		return (bufferIndex * bufferSize_) + index;
	}

public:
	AlignedAllocator() : AlignedAllocator(1024) { }

	AlignedAllocator(size_t bufferSize) : buffers_(), bufferSize_(bufferSize), insertIndex_({ 0, 0 })
	{
		printf("AlignedAllocater initialized with buffersize: %zu\r\n", bufferSize);
		buffers_.push_back(allocBuffer());
	}

	size_t size()
	{
		return (insertIndex_.buffer * bufferSize_) + insertIndex_.index;
	}

	size_t alloc()
	{
		size_t i = insertIndex_.index++;
		size_t bi = insertIndex_.buffer;

		if (insertIndex_.index == bufferSize_)
		{
			insertIndex_.index = 0;
			insertIndex_.buffer++;
			buffers_.push_back(allocBuffer());
		}

		return calcIndex(bi, i);
	}

	void free(size_t index)
	{
		const size_t lastIndex = calcIndex(insertIndex_.buffer, insertIndex_.index) - 1;

		if (index > lastIndex)
		{
			printf("Cannot free at index %zu!", index);
			return;
		}

		T* oldPtr = at(insertIndex_.buffer, insertIndex_.index);

		if (index != lastIndex)
		{
			T* newPtr = at(index);
			*newPtr = *oldPtr;
		}

		insertIndex_ = calcIndexStruct(lastIndex);
		memset(oldPtr, 0, sizeof(T));
	}

	template<typename Callback>
	void free(size_t index, Callback onMove)
	{
		const size_t lastIndex = calcIndex(insertIndex_.buffer, insertIndex_.index) - 1;

		if (index < 0 && index > lastIndex)
		{
			printf("Cannot free at index %zu!", index);
			return;
		}

		T* oldPtr = at(lastIndex);

		if (index != lastIndex)
		{
			T* newPtr = at(index);
			*newPtr = *oldPtr;
			onMove(lastIndex, index, newPtr);
		}

		insertIndex_ = calcIndexStruct(lastIndex);
		memset(oldPtr, 0, sizeof(T));
	}

	inline T* at(size_t bufferIndex, size_t index)
	{
		T* buffer = buffers_.data()[bufferIndex];
		return &buffer[index];
	}

	inline T* at(Index index)
	{
		T* buffer = buffers_.data()[index.buffer];
		return &buffer[index.index];
	}

	inline T* at(size_t index)
	{
		return at(calcIndexStruct(index));
	}

	template<typename Callback>
	void iterate(Callback callback)
	{
		size_t l = buffers_.size();
		size_t indexCounter = 0;
		T* buffer;
		for (size_t i = 0; i < l; i++)
		{
			buffer = buffers_.data()[i];
			for (size_t j = 0; j < bufferSize_; j++)
			{
				callback(&buffer[j], indexCounter++);
			}
		}
	}
};