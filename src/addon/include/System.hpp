#pragma once

class Engine;

class System
{
protected:
	Engine* const engine;

private:
	System(Engine* const engine);

	friend class Engine;
};