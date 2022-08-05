#pragma once

#include "SubSystem.hpp"
#include "Hasher.hpp"
#include "systems/RenderSystem.hpp"

class Renderer : public SubSystem
{
private:
	RenderSystem renderSystem;
	
	SUB_SYSTEM_OVERRIDES(Renderer, renderSystem());
public:
	void render();
};