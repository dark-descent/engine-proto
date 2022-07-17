#pragma once

#include "pch.hpp"

#include "v8_helpers.hpp"

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__; __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__; __pragma(pack(pop))
#endif

template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type { };

template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type { };

inline void* aligned_malloc(size_t alignment, size_t size)
{
#ifdef _MSC_VER
	return _aligned_malloc(size, alignment);
#else
	void* ptr = nullptr;
	return posix_memalign(&ptr, alignment, size);
#endif
}

template<typename T>
inline T* aligned_malloc(size_t alignment, size_t size)
{
	return static_cast<T*>(aligned_malloc(alignment, size));
}