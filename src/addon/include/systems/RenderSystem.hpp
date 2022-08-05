#pragma once

#include "System.hpp"
#include "components/Transform.hpp"

class RenderSystem : public System
{
public:
	// RenderSystem();
	
	virtual void run(ArchGroup& archTypes, const size_t count) override;
	virtual void initialize(Engine& engine) override;
};