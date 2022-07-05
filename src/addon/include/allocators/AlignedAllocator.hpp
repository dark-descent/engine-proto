#pragma once

#include <vector>
#include <stdlib.h>

typedef void(*RellocateCalback)(size_t, void*, void*);

class AlignedAllocator
{
private:
	struct Index
	{
		size_t buffer;
		size_t index;
	};

	std::vector<void*> buffers_;
	Index insertIndex_ = { 0, 0 };
	size_t itemSize_;
	size_t bufferSize_;

	RellocateCalback rellocateCalback_;
	void* callbackData_;

	inline void* allocBuffer()
	{

#ifdef _MSC_VER
		return _aligned_malloc(itemSize_ * bufferSize_, 16);
#else
		void* ptr = nullptr;
		return posix_memalign(&ptr, 16, itemSize_ * bufferSize_);
#endif
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
	AlignedAllocator(size_t itemSize, size_t bufferSize, RellocateCalback callback, void* callbackData) : itemSize_(itemSize), buffers_(), bufferSize_(bufferSize), insertIndex_({ 0, 0 }), rellocateCalback_(callback), callbackData_(callbackData)
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
		const Index idx = calcIndexStruct(index);
		// printf("free at index %zu [%zu, %zu]\r\n", index, idx.buffer, idx.index);

		const int32_t bi = idx.buffer;
		const int32_t i = idx.index;

		int32_t lbi = insertIndex_.buffer;
		int32_t li = insertIndex_.index - 1;

		if (li < 0)
		{
			lbi--;
			li = bufferSize_ - 1;
		}

		size_t bufferPtr = reinterpret_cast<size_t>(buffers_[bi]) + (i * itemSize_);
		void* freedPtr = reinterpret_cast<void*>(bufferPtr);
		size_t lastBufferPtr = reinterpret_cast<size_t>(buffers_[lbi]) + (li * itemSize_);
		void* lastPtr = reinterpret_cast<void*>(lastBufferPtr);

		if (freedPtr != lastPtr)
		{
			memcpy(freedPtr, lastPtr, itemSize_);

			if (rellocateCalback_ != nullptr)
				rellocateCalback_(index, freedPtr, callbackData_);
		}

		insertIndex_.buffer = lbi;
		insertIndex_.index = li;
	}

	inline void* at(size_t bufferIndex, size_t index)
	{
		return reinterpret_cast<void*>(reinterpret_cast<size_t>(buffers_[bufferIndex]) + (index * itemSize_));
	}

	inline void* at(Index index)
	{
		return at(index.buffer, index.index);
	}

	inline void* at(size_t index)
	{
		return at(index / bufferSize_, index % bufferSize_);
	}

	template<typename Callback>
	void iterate(Callback callback)
	{
		size_t l = buffers_.size();
		size_t indexCounter = 0;
		void* buffer;
		for (size_t i = 0; i < l; i++)
		{
			buffer = buffers_[i];
			for (size_t j = 0; j < bufferSize_; j++)
			{
				callback(&buffer[j], indexCounter++);
			}
		}
	}
};
