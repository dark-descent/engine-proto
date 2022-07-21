#include "systems/RenderSystem.hpp"
#include "components/Transform.hpp"
#include "Engine.hpp"

// RenderSystem::RenderSystem() { }

void RenderSystem::initialize(Engine& engine)
{
	query_.required |= engine.getComponent<Transform>().getBitMask();
}

void RenderSystem::run(ArchGroup& archTypes, const size_t count)
{
	for(auto& arch : archTypes)
	{
		printf("got arch allocator for %zu\n", arch->bitMask);
	}
}