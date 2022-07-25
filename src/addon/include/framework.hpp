#pragma once

#include <node.h>

#include <array>
#include <queue>
#include <map>
#include <unordered_map>
#include <vector>
#include <stack>
#include <utility>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <format>
#include <thread>
#include <mutex>
#include <condition_variable>

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

inline void aligned_free(void* ptr)
{
#ifdef _MSC_VER
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}

template<typename T>
inline void aligned_free(T* ptr)
{
#ifdef _MSC_VER
	_aligned_free(static_cast<void*>(ptr));
#else
	free(static_cast<void*>(ptr));
#endif
}
