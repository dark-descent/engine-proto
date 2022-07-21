#include "Renderer.hpp"

#include "Engine.hpp"

void Renderer::onInitialize(Config& config, ObjectBuilder& exports)
{
	engine.logger.info("Renderer initialized!");
	renderSystem.initialize(engine);
}

void Renderer::onTerminate()
{

}

void Renderer::render()
{
	engine.runSystem(renderSystem);
}