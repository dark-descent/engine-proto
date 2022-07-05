#include "gfx/Renderer.hpp"
#include "Engine.hpp"

void gfx::Renderer::onInitialize(const Config& config)
{
	emitEvent("frameReady", [](ObjectBuilder& builder)
	{
		builder.set("test", true);
	});
}

void gfx::Renderer::onDestroy()
{
	
}