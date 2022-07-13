#pragma once

#include <stdint.h>

namespace Enum
{
	template<typename T, typename IntType = uint64_t>
	constexpr T Or(T a, T b)
	{
		return static_cast<T>(static_cast<IntType>(a) | static_cast<IntType>(b));
	}

	template<typename T, typename IntType = uint64_t>
	constexpr T And(T a, T b)
	{
		return static_cast<T>(static_cast<IntType>(a) & static_cast<IntType>(b));
	}

	template<typename T, typename IntType = uint64_t>
	constexpr T Xor(T a, T b)
	{
		return static_cast<T>(static_cast<IntType>(a) ^ static_cast<IntType>(b));
	}

	template<typename T, typename IntType = uint64_t>
	constexpr T Nor(T a, T b)
	{
		return static_cast<T>(~(static_cast<IntType>(a) | static_cast<IntType>(b)));
	}

	template<typename T, typename IntType = uint64_t>
	constexpr bool Has(T a, T b)
	{
		return And(a, b) == b;
	}

	template<typename T = uint64_t>
	constexpr T Cast(auto enumValue)
	{
		return static_cast<T>(enumValue);
	}

	template<typename T>
	constexpr T Wrap(auto val)
	{
		return static_cast<T>(val);
	}
};