#pragma once

using Hash = size_t;

struct Hasher
{
private:
	constexpr static Hash hashString(const char* str)
	{
		size_t hash = 5381;
		int c = 0;

		while ((c = *str++))
			hash = ((hash << 5) + hash) * 33 + c;

		return hash;
	}

public:
	constexpr static Hash hash(const char* str) { return hashString(str); }
	constexpr static bool check(Hash& hashStr, const char* str) { return hash(str) == hashStr; }
};