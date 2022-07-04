#pragma once

#include "Component.hpp"
#include "Vector2.hpp"

PACK(struct Transform : public Component
{
	Vector2 position;
	Vector2 rotation;
	Vector2 scale;
});
