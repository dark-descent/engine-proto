#pragma once

#include "System.hpp"

class Engine;

namespace gfx
{
	class Renderer : public System
	{
		SYSTEM_CTOR(Renderer)
		SYSTEM_IMPL()

		friend class Engine;
	};
}