#pragma once

#include "framework.hpp"

#include "Component.hpp"
#include "Logger.hpp"

using ArchTypeIndex = size_t;

class ArchType : public Logger::ILog
{
public:
	ArchTypeIndex index;
	size_t bitMask;
	size_t size;
	std::vector<ArchTypeIndex> remove;
	std::vector<ArchTypeIndex> add;
	std::vector<ComponentLayout> componentLayouts;

	ArchType(ArchTypeIndex index, size_t bitMask, size_t size, std::vector<ComponentLayout>& layout);

	void* alloc();

	void log();
};
