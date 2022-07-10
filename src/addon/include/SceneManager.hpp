#pragma once

#include "SubSystem.hpp"
#include "Hasher.hpp"

SUB_SYSTEM_CLASS(SceneManager)
{
	SUB_SYSTEM_OVERRIDES(SceneManager);

public:
	void loadScene(Hash sceneName);
};