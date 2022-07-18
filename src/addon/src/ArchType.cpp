#include "ArchType.hpp"
#include <format>

ArchType::ArchType(ArchTypeIndex index, size_t bitMask, size_t size, std::vector<ComponentLayout>& componentLayouts) : index(index), bitMask(bitMask), size(size), remove(), add(), componentLayouts(std::move(componentLayouts))
{

}

void* ArchType::alloc()
{
	return nullptr;
}

void ArchType::log()
{
	printf(
	"{\n"
	"    index: %zu,\n"
	"    bitMask: %zu,\n"
	"    size: %zu\n"
	"}", 
	index, bitMask, size);
}